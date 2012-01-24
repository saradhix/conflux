#include "connection.h"
#include "memory.h"
#include <unistd.h>

struct flux_connection *flux_connection_init(int sock ,struct sockaddr_in *client_addr)
{
	struct flux_connection *connection;

	connection = flux_malloc(sizeof(struct flux_connection));
	if(!connection) return NULL;
	memset(connection,0,sizeof(struct flux_connection));

	connection->sock = sock;
	strcpy(connection->ip,inet_ntoa(client_addr->sin_addr));
	connection->port=ntohs(client_addr->sin_port);
	return connection;
}

void flux_connection_cleanup(struct flux_connection *connection)
{
	if(!connection) return;

	if(connection->sock != -1){
		close(connection->sock);
	}
	flux_free(connection);
}
int print_connections(struct flux_connection **c,int howmany)
{
	int i,j;
	for(i=0;i<howmany;i++)
	{
		if(c[i])
		{
			snprintf(confluxlog,sizeof(confluxlog),"i=%d sock=%d ip=%s port=%d state=%d",i,c[i]->sock,c[i]->ip,c[i]->port,c[i]->state);
			conflux_log(confluxlog);
			if(c[i]->subscription_count)
			{
				for(j=0;j<c[i]->subscription_count;j++)
				{
					snprintf(confluxlog,sizeof(confluxlog),"j=%d name=%s",j,c[i]->subscriptions[j].sub_name);
					conflux_log(confluxlog);
				}
			}
		}
	}
}
