#define MAX_NAME_LENGTH 16
#define MAX_IP_LENGTH 60
#define MAX_SUBSCRIPTIONS 100
#define MAX_SUBSCRIPTION_LENGTH 32
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "log.h"

struct subscription
{
	char sub_name[MAX_SUBSCRIPTION_LENGTH];
	int is_valid;
	int msgs_rx;
};
struct flux_connection
{
	int sock;
	char name[MAX_NAME_LENGTH];
	char ip[MAX_IP_LENGTH];
	int port;
	int state;
	struct subscription subscriptions[MAX_SUBSCRIPTIONS];
	int subscription_count;
};

struct flux_connection *flux_connection_init(int sock,struct sockaddr_in *client);
void flux_connection_cleanup(struct flux_connection *connection);
