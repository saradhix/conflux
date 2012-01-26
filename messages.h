#define CONNECT 1
#define DISCONNECT 2
#define SUBSCRIBE 3
#define PUBLISH 4

#define CONNECTED 1
#define DISCONNECTED 2
#define SUBSCRIBED 3
#define PUBLISHED 4

#define MAX_NAME_LEN 32
#define MAX_TOPIC_LEN 32

struct flux_connect
{
	int command;
	char name[16];
};

struct flux_disconnect
{
	int command;
	char name[16];//do you need this ??
};
struct flux_publish
{
	int command;
	char topic[16];
	int len;
	char payload[256];
};

struct flux_subscribe
{
	int command;
	char topic[16];
	int len;
};
