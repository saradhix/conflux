#define CONNECT 1
#define DISCONNECT 2
#define SUBSCRIBE 3
#define PUBLISH 4

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
