#include <string.h>
#include <stdio.h>
#include "messages.h"


int send_connect_msg(int fd, char *id)
{
	int ret=0;
	struct flux_connect connect_msg;
	connect_msg.command=CONNECT;
	strncpy(connect_msg.name,id,16);
	connect_msg.name[15]=0;
	ret=send(fd,(void *)&connect_msg,sizeof(connect_msg),0);
	return ret;
}

int send_publish_msg(int fd, char *topic, void *payload, int payload_length)
{
	int total_length=4+4+strlen(topic)+4+payload_length;
	char buffer[100];
	char *ptr=buffer;
	int ret=0;
	int command=PUBLISH,topic_len=0;
	topic_len=strlen(topic);
	printf("PL=%d TL=%d\n",payload_length,topic_len);

	/*4 bytes of command*/
	memcpy(ptr,(char *)&command,4);
	ptr+=4;
	/*Encode topic length*/
	memcpy(ptr,(char *)&topic_len,4);
	ptr+=4;
	/*Encode topic*/
	memcpy(ptr,topic,topic_len);
	ptr+=topic_len;
	/*Encode payload len*/
	memcpy(ptr,(char *)&payload_length,4);
	ptr+=4;
	printf("Payload_length=%d total=%d\n",payload_length,total_length);
	/*Encode the payload*/
	memcpy(ptr,(char *)payload,payload_length);
	ptr+=payload_length;
	ret=send(fd,(void *)buffer,total_length,0);
	printf(" Bytes sent =Ret=%d\n",ret);
	//print_bytes(buffer,total_length);
	return ret;
}

int send_subscribe_msg(int fd, char *topic)
{
	int ret=0;
	char buffer[100];
	char *ptr=buffer;
	int total_length,topic_len;
	int command=SUBSCRIBE;
	topic_len=strlen(topic);
	/*4 bytes of command*/
	memcpy(ptr,(char *)&command,4);
	ptr+=4;
	/*Encode topic length*/
	memcpy(ptr,(char *)&topic_len,4);
	ptr+=4;
	/*Encode topic*/
	memcpy(ptr,topic,topic_len);
	ptr+=topic_len;
	total_length=4+4+topic_len;
	ret=send(fd,buffer,total_length,0);
	return ret;
}

int send_unsubscribe_msg(int fd, char *topic)
{
	int ret=0;
	char buffer[100];
	char *ptr=buffer;
	int total_length,topic_len;
	int command=UNSUBSCRIBE;
	topic_len=strlen(topic);
	/*4 bytes of command*/
	memcpy(ptr,(char *)&command,4);
	ptr+=4;
	/*Encode topic length*/
	memcpy(ptr,(char *)&topic_len,4);
	ptr+=4;
	/*Encode topic*/
	memcpy(ptr,topic,topic_len);
	ptr+=topic_len;
	total_length=4+4+topic_len;
	ret=send(fd,buffer,total_length,0);
	return ret;
}


int nrecv(int fd,void *buf,int nbytes,int flags)
{
	int nleft, nread;
	nleft=nbytes;

	while (nleft > 0)
	{
		nread=recv(fd,buf,nleft,flags);
		if (nread< 0) return nread;
		if(nread==0) break;
		nleft-=nread;
		buf+=nread;
	}
	return (nbytes-nleft);
}
int nsend(int fd,void *buf,int nbytes,int flags)
{
	int nleft, nwritten;
	nleft=nbytes;

	while (nleft > 0)
	{
		nwritten=send(fd,buf,nleft,flags);
		if (nwritten< 0) return nwritten;
		if(nwritten==0) break;
		nleft-=nwritten;
		buf+=nwritten;
	}
	return (nbytes-nleft);
}

int print_bytes(char *buf,int howmany)
{
	int i;
	for(i=0;i<howmany;i++)
	{
		printf("i=%d charval=%c intval=%d hex=%x\n",i,buf[i],buf[i],buf[i]);
	}
}
