#include "connection.h"
#include "messages.h"
#include "net.h"
#include <string.h>

extern int connection_count;
/*
extern struct flux_connection **connections;
*/
int flux_handle_connect_message(struct flux_connection *conn)
{
	int sock=0,ret=0;
	char name[MAX_NAME_LEN]={0};
	sock=conn->sock;
	/*We have already read the command. Rest of the content in the flux_connect should be read*/
	ret=recv(sock,name,sizeof(struct flux_connect)-sizeof(int),0);
	if(ret<=0) return ret;
	slog(LOG_INFO,"Name of the agent : %s",name);
	return ret;

}

int flux_handle_publish_message(struct flux_connection *conn,struct flux_connection **connections)
{
	int sock=0,ret=0,i,j;
	char topic[MAX_TOPIC_LEN]={0};
	char payload[1000]={0};
	int topic_len=0,payload_len=0;
	sock=conn->sock;
	/*We have already read the command. Rest of the content in the flux_connect should be read*/
	/*Read topic length*/
	ret=recv(sock,(char *)&topic_len,4,0);
	if(ret<=0) return ret;
	slog(LOG_INFO,"Topic length=%d",topic_len);
	/*Read topic*/
	ret=recv(sock,topic,topic_len,0);
	if(ret<=0) return ret;
	slog(LOG_DEBUG,"Topic=%s",topic);
	/*Read payload length*/
	ret=recv(sock,(char *)&payload_len,4,0);
	if(ret<=0) return ret;
	slog(LOG_DEBUG,"Payload len=%d",payload_len);
	/*Read the payload*/
	ret=recv(sock,payload,payload_len,0);
	if(ret<=0) return ret;
	slog(LOG_INFO,"Received from sock=%d tl=%d topic=%s pl=%d payload=%s",sock,topic_len,topic,payload_len,payload);


/*Now route the message*/

	for(i=0;i<connection_count;i++)
	{
		if(connections[i]==NULL) continue;
		for(j=0;j<connections[i]->subscription_count;j++)
		{
			if(strcmp(connections[i]->subscriptions[j].sub_name,topic)==0)
			{
				slog(LOG_INFO,"Sending message to %d",connections[i]->sock);
				send_publish_msg(connections[i]->sock,topic,payload,payload_len);
			}	
		}
	}

	return ret;

}

int flux_handle_subscribe_message(struct flux_connection *conn)
{
	int sock=0,ret=0;
	char topic[100]={0};
	int topic_length=0;
	sock=conn->sock;
	/*We have already read the command. Rest of the content in the flux_connect should be read*/

	/*Read the topic length*/
	ret=recv(sock,(char *)&topic_length,4,0);
	if(ret<=0) return ret;
	slog(LOG_INFO,"Topic length : %d",topic_length);

	/*Read the topic now*/
	ret=recv(sock,topic,topic_length,0);
	if(ret<=0) return ret;
	slog(LOG_INFO,"Topic =%s",topic);
	strcpy(conn->subscriptions[conn->subscription_count].sub_name,topic);
	conn->subscriptions[conn->subscription_count].is_valid=1;
	conn->subscription_count++;

	return ret;

}
int flux_handle_unsubscribe_message(struct flux_connection *conn)
{
	int sock=0,ret=0;
	char topic[100]={0};
	int topic_length=0,i;
	sock=conn->sock;
	/*We have already read the command. Rest of the content in the flux_connect should be read*/

	/*Read the topic length*/
	ret=recv(sock,(char *)&topic_length,4,0);
	if(ret<=0) return ret;
	slog(LOG_INFO,"Topic length : %d",topic_length);

	/*Read the topic now*/
	ret=recv(sock,topic,topic_length,0);
	if(ret<=0) return ret;
	slog(LOG_INFO," Unsubscribed Topic =%s",topic);

	/*Search for this topic and make the is_valid field for that particular sub as 0*/
	for(i=0;i<conn->subscription_count;i++)
	{
		slog(LOG_DEBUG,"Checking %s and %s",conn->subscriptions[i].sub_name,topic);
		if(strcmp(conn->subscriptions[i].sub_name,topic)==0)
		{
			conn->subscriptions[i].is_valid=0;
		}
	}

	return ret;
}

int flux_handle_disconnect_message(struct flux_connection *conn)
{
}
