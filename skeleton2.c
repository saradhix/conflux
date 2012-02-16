/* tcpclient.c */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "messages.h"
#include "net.h"

int default_publish_handler(char *topic, void *payload, int length);
int main()
{

	int sock, choice;  
	char send_data[1024]="Hello world\n",recv_data[1024];
	struct hostent *host;
	struct sockaddr_in server_addr;  
	char buff[160], payload[160],topic[16];
	int i,ret,cx=0;
	int topic_len=0,payload_len=0;
	fd_set readfds,writefds;
	struct timeval timeout;
	time_t t;
	int fdcount=0;

	host = gethostbyname("127.0.0.1");

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}

	server_addr.sin_family = AF_INET;     
	server_addr.sin_port = htons(5000);   
	server_addr.sin_addr = *((struct in_addr *)host->h_addr);
	bzero(&(server_addr.sin_zero),8); 

	if (connect(sock, (struct sockaddr *)&server_addr,
				sizeof(struct sockaddr)) == -1) 
	{
		perror("Connect");
		exit(1);
	}
	while(1)
	{
		FD_ZERO(&readfds);
		FD_SET(sock,&readfds);
		timeout.tv_sec = 5;
		timeout.tv_usec=0;
		printf("Blocking at pselect with sockmax=%d\n",sock);
		fdcount = select(sock+1, &readfds, NULL, NULL, &timeout);
		if(fdcount == -1)
		{
			printf("Some error might have occured\n");
		}
		else
		{
			printf("Value of ret=%d\n",fdcount);
			if(FD_ISSET(sock,&readfds))
			{
				printf("Some message received from %d\n",sock);
				if(skeleton_process_message(sock)<=0)
				{
					skeleton_process_close_message(sock,&readfds);
				}
			}
		}
		//do the periodic activity of publishing a test message
		sprintf(topic,"%s","TEST");
		sprintf(buff,"Time is now %d",time(&t));
		send_publish_msg(sock,topic,buff,strlen(buff));
		printf("Published message on topic %s with payload %s\n",topic,buff);
	}

	close(sock);

	return 0;
}


int skeleton_process_message(int socket)
{
	char buff[100]={0};
	int command;
	int ret=0;
	ret=recv(socket,(char *)&command,4,0);
	/*Receive the 4 bytes command first. Based on it call appropriate handler function*/
	if(ret<=0)
	{
		printf("Socket %d iss closed\n",socket);
		return ret;
	}

	printf("ret=%d Command=%d from fd=%d\n",ret,command,socket);
	//now go ahead and parse the remaining bytes
	switch(command)
	{
		case PUBLISH:
			ret=skeleton_flux_handle_publish_message(socket,default_publish_handler);
			break;


	}
	printf("Returning value %d\n",ret);
	return ret;
}
int skeleton_flux_handle_publish_message(int sock, int (*func)(char *,void *,int))
{
	int ret=0,i,j;
	char topic[MAX_TOPIC_LEN]={0};
	char payload[1000]={0};
	int topic_len=0,payload_len=0;
	/*We have already read the command. Rest of the content in the flux_connect should be read*/
	/*Read topic length*/
	ret=recv(sock,(char *)&topic_len,4,0);
	if(ret<=0) return ret;
	printf("Topic length=%d\n",topic_len);
	/*Read topic*/
	ret=recv(sock,topic,topic_len,0);
	if(ret<=0) return ret;
	printf("Topic=%s\n",topic);
	/*Read payload length*/
	ret=recv(sock,(char *)&payload_len,4,0);
	if(ret<=0) return ret;
	printf("Payload len=%d\n",payload_len);
	/*Read the payload*/
	ret=recv(sock,payload,payload_len,0);
	if(ret<=0) return ret;
	printf("Value of ret=%d\n",ret);
	printf("Received from sock=%d tl=%d topic=%s pl=%d payload=%s\n",sock,topic_len,topic,payload_len,payload);
	func(topic,payload,payload_len);
}

int default_publish_handler(char *topic, void *payload, int length)
{
	printf("Received topic=%s\n",topic);
	printf("Received payload_len=%d\n",length);
	printf("Received payload=%s\n",(char *)payload);
	return length;
}
int skeleton_process_close_message(int sock)
{
	//this is dummy for now
	printf("Socket sock got closed from server\n");
	close(sock);
	printf("Flux process death. Unable to handle..Hence dieing\n");
	exit(1);
	return 0;
}
