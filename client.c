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

int main()
{

	int sock, choice;  
	char send_data[1024]="Hello world\n",recv_data[1024];
	struct hostent *host;
	struct sockaddr_in server_addr;  
	char buff[10];
	int i,ret;
	struct flux_connect connect_msg;
	struct flux_publish publish_msg;
	struct flux_subscribe subscribe_msg;

	connect_msg.command=CONNECT;
	strcpy(connect_msg.name,"hello");
	publish_msg.command=PUBLISH;	
	strcpy(publish_msg.topic,"testtopic");
	publish_msg.len=32;
<<<<<<< HEAD
	strcpy(publish_msg.payload,"Testpayload");
=======
>>>>>>> ef6f8849135ebaa1d157b01a345be7072372915d
	subscribe_msg.command=SUBSCRIBE;	
	strcpy(subscribe_msg.topic,"testtopic2");
	subscribe_msg.len=32;

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
				printf("Sending byf-%s\n",send_data);

				ret=send(sock,(void *)&connect_msg,sizeof(connect_msg), 0); 
				break;
			case 2:
				ret=send(sock,(void *)&publish_msg,sizeof(publish_msg), 0); 
				break;
			case 3:
				ret=send(sock,(void *)&subscribe_msg,sizeof(subscribe_msg), 0); 
				break;
		}
		if(ret<=0) perror("send");
	}

	close(sock);

	return 0;
}
