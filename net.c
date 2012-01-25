#include <string.h>
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
	int ret=0;
	struct flux_publish publish_msg;
	publish_msg.command=PUBLISH;
	publish_msg.len=32;
	strncpy(publish_msg.topic,topic,16);
	publish_msg.len=payload_length;
	strncpy(publish_msg.payload,(char *)payload,payload_length);
	ret=send(fd,(void *)&publish_msg,sizeof(publish_msg),0);
	return ret;
}

int send_subscribe_msg(int fd, char *topic)
{
	int ret=0;
	struct flux_subscribe subscribe_msg;
	subscribe_msg.command=SUBSCRIBE;
	strncpy(subscribe_msg.topic,topic,16);
	subscribe_msg.len=16;
	ret=send(fd,(void *)&subscribe_msg,sizeof(subscribe_msg),0);
}
