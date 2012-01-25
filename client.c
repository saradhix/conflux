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

int main()
{

	int sock, choice;  
	char send_data[1024]="Hello world\n",recv_data[1024];
	struct hostent *host;
	struct sockaddr_in server_addr;  
	char buff[160],topic[16];
	int i,ret,cx=0;

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
		printf("Press enter to send data\n");
		scanf("%d",&choice); 
		switch(choice)
		{
			case 1:
				printf("Sending connect choice\n");
				sprintf(buff,"Name %d",cx++);
				ret=send_connect_msg(sock,buff);
				break;
			case 2:
				printf("Sending publish message\n");
				sprintf(topic,"Topic %d",cx++);
				sprintf(buff,"This is message number %d",cx);
				ret=send_publish_msg(sock,topic,buff,strlen(buff));
				break;
			case 3:
				printf("Sending subscribe message\n");
				sprintf(topic,"Subtopic%d",cx++);
				ret=send_subscribe_msg(sock,topic);
				break;
		}
		if(ret<=0) perror("send");
	}

	close(sock);

	return 0;
}



