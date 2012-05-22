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
#include "memory.h"
#include "config.h"

#define TIMEOUT 5
#define MAX 20

int connection_count=10;
struct config server={0};
int main()
{

	int true=1,new_sock;
	struct timeval timeout;
	struct flux_connection **connections=NULL, *new_connection=NULL;


	int sock,fdsock,fdcount;  
	int i,ret,count=0,size=sizeof(struct sockaddr);

	struct sockaddr_in server_addr,client_addr;    
	int sin_size;
	fd_set readfds,writefds;
	int sockmax;
	sigset_t sigblock;

	/*Set the log leve*/
	server.loglevel=LOG_INFO;
	server.logfile=NULL;
	server.syslog_enabled=0;

	/*Initialise all connections to null*/

	connections=flux_malloc(sizeof(struct flux_connection *)*connection_count);
	memset(connections,0,sizeof(connections[0])*connection_count);

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

	printf("Flux started on port 5000!\n");
	sigemptyset(&sigblock);
	sigaddset(&sigblock, SIGINT);


	while(1)
	{  

		FD_ZERO(&readfds);
		FD_SET(sock,&readfds);
		sockmax=sock;

		for(i=0;i<connection_count;i++)
		{
			if(connections[i] && connections[i]->sock !=-1)
			{
				FD_SET(connections[i]->sock,&readfds);
				/*Add this socket to the fd set*/
				if(connections[i]->sock > sockmax) sockmax=connections[i]->sock;
			}
		}
		timeout.tv_sec = 5;
		timeout.tv_usec=0;
		slog(LOG_DEBUG,"Blocking at pselect with sockmax=%d",sockmax);
		fdcount = select(sockmax+1, &readfds, &writefds, NULL, &timeout);
		if(fdcount == -1)
		{
			slog(LOG_INFO,"Some error might have occured");
		}
		else
		{
			for(i=0;i<connection_count;i++)
			{
				if(connections[i] && connections[i]->sock !=-1 && FD_ISSET(connections[i]->sock,&readfds))
				{
					slog(LOG_DEBUG,"Some message received from %d",connections[i]->sock);
					if(process_message(connections[i],connections)<=0)
					{
						process_close_message(connections[i],&readfds);
					}
				}
			}
		}
		if(FD_ISSET(sock, &readfds))
		{
			new_sock = accept(sock, (struct sockaddr *)&client_addr, &size);
			if(new_sock <0) perror("accept");
			slog(LOG_INFO,"New connection with size=%d ip=%s port=%d",size,inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
			new_connection = flux_connection_init(new_sock,&client_addr);
			//strcpy(new_connection->ip,inet_ntoa(client_addr.sin_addr));
			//new_connection->port=ntohs(client_addr.sin_port);
			for(i=0; i<connection_count; i++)
			{
				if(connections[i] == NULL)
				{
					connections[i] = new_connection;
					slog(LOG_INFO,"New fd= %d",connections[i]->sock);
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

int process_message(struct flux_connection *conn,struct flux_connection **connections)
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
		slog(LOG_INFO,"Socket %d iss closed",socket);
		return ret;
	}

	slog(LOG_INFO,"ret=%d Command=%d from fd=%d",ret,command,socket);
	//now go ahead and parse the remaining bytes
	switch(command)
	{
		case CONNECT:
			ret=flux_handle_connect_message(conn);
			return ret;
			break;
		case DISCONNECT:break;
		case PUBLISH:
			ret=flux_handle_publish_message(conn,connections);
			break;
		case SUBSCRIBE:
			ret=flux_handle_subscribe_message(conn);
			break;
		case UNSUBSCRIBE:
			ret=flux_handle_unsubscribe_message(conn);
			break;


	}
	return ret;
}


int process_close_message(struct flux_connection *conn,fd_set *fdset)
{
	FD_CLR(conn->sock,fdset);
	close(conn->sock);
	conn->sock=-1;
	return 0;
}


