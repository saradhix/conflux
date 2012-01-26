#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#include "connection.h"
#include "messages.h"

#define TIMEOUT 5
#define MAX 20


int main()
{

	struct flux_connection **connections=NULL, *new_connection=NULL;
	int connection_count=10,true=1,new_sock;
	struct timeval timeout;

	int sock,fdsock,fdcount;  
	int i,ret,count=0,size=sizeof(struct sockaddr);

	struct sockaddr_in server_addr,client_addr;    
	int sin_size;
	fd_set readfds,writefds;
	int sockmax;
	sigset_t sigblock;

	/*Initialise all connections to null*/

	connections=flux_malloc(sizeof(struct flux_connection *)*connection_count);
	memset(connections,0,4*connection_count);

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Socket");
		exit(1);
	}

	if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&true,sizeof(int)) == -1)
	{
		perror("Setsockopt");
		exit(1);
	}

	server_addr.sin_family = AF_INET;         
	server_addr.sin_port = htons(5000);     
	server_addr.sin_addr.s_addr = INADDR_ANY; 
	bzero(&(server_addr.sin_zero),8); 

	if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("Unable to bind\n");
		exit(1);
	}

	if (listen(sock, 5) == -1) {
		perror("Listen");
		exit(1);
	}

	printf("TCPServer Waiting for client on port 5000\n");
	sigemptyset(&sigblock);
	sigaddset(&sigblock, SIGINT);


	while(1)
	{  

		FD_ZERO(&readfds);
		FD_SET(sock,&readfds);
		sockmax=sock;

		for(i=0;i<connection_count;i++)
		{
			//if(connections[i])printf("i=%d sock=%d\n",i,connections[i]->sock);

			if(connections[i] && connections[i]->sock !=-1)
			{
				FD_SET(connections[i]->sock,&readfds);
				/*Add this socket to the fd set*/
				//printf("Adding %d to list\n",connections[i]->sock);
				if(connections[i]->sock > sockmax) sockmax=connections[i]->sock;
			}
		}
		timeout.tv_sec = 5;
		timeout.tv_usec=0;
		printf("Blocking at pselect with sockmax=%d\n",sockmax);
		fdcount = select(sockmax+1, &readfds, &writefds, NULL, &timeout);
		if(fdcount == -1)
		{
			printf("Some error might have occured\n");
		}
		else
		{
			for(i=0;i<connection_count;i++)
			{
				if(connections[i] && connections[i]->sock !=-1 && FD_ISSET(connections[i]->sock,&readfds))
				{
					printf("Some message received from %d\n",connections[i]->sock);
					if(process_message(connections[i])<=0)
					{
						process_close_message(connections[i],&readfds);
					}
				}
			}
		}
		if(FD_ISSET(sock, &readfds))
		{
			printf("Blocking at accept\n");
			new_sock = accept(sock, (struct sockaddr *)&client_addr, &size);
			if(new_sock <0) perror("accept");
			printf("New connection with size=%d ip=%s port=%d\n",size,inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
			new_connection = flux_connection_init(new_sock,&client_addr);
			//strcpy(new_connection->ip,inet_ntoa(client_addr.sin_addr));
			//new_connection->port=ntohs(client_addr.sin_port);
			for(i=0; i<connection_count; i++)
			{
				if(connections[i] == NULL)
				{
					connections[i] = new_connection;
					printf("New fd= %d \n",connections[i]->sock);
					break;
				}
			}
			/*Code for more than 100 clients connecting for future*/

		}
		print_connections(connections,connection_count);

	}       

	close(sock);
	return 0;
}

int process_message(struct flux_connection *conn)
{
	char buff[100]={0};
	int command;
	int ret=0;
	int socket=conn->sock;
	struct flux_connect connect_msg;
	struct flux_publish publish_msg;
	struct flux_subscribe subscribe_msg;
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
		case CONNECT:
			//ret=recv(socket,connect_msg.name,sizeof(connect_msg)-4,0);
			ret=flux_handle_connect_message(conn);
			return ret;
			//printf("Received connect message with name %s\n",connect_msg.name);
			break;
		case DISCONNECT:break;
		case PUBLISH:
			//publish_msg.command=command;
//read topicname and length now
			/*ret=recv(socket,publish_msg.topic,16,0);
			ret=recv(socket,&publish_msg.len,4,0);
			ret=recv(socket,&publish_msg.payload,256,0);*/
			ret=flux_handle_publish_message(conn);
			return ret;
			//printf("Received publish message with topic %s and length=%d and payload=%s\n",publish_msg.topic,publish_msg.len,publish_msg.payload);
			break;
		case SUBSCRIBE:
			/*subscribe_msg.command=command;
			ret=recv(socket,subscribe_msg.topic,16,0);
			ret=recv(socket,&subscribe_msg.len,4,0);
			if(ret<=0) return ret;*/
			ret=flux_handle_subscribe_message(conn);
			return ret;
//			printf("Received subscribe message with topic %s and length=%d\n",subscribe_msg.topic,subscribe_msg.len);
			strcpy(conn->subscriptions[conn->subscription_count++].sub_name,subscribe_msg.topic);
			break;
	}
	printf("Returning value %d\n",ret);
	return ret;
}


int process_close_message(struct flux_connection *conn,fd_set *fdset)
{
	FD_CLR(conn->sock,fdset);
	close(conn->sock);
	conn->sock=-1;
	return 0;
}


