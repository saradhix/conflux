#include "connection.h"
#include <unistd.h>

struct flux_connection *flux_connection_init(int sock)
{
	struct flux_connection *connection;

	connection = flux_malloc(sizeof(struct flux_connection));
	if(!connection) return NULL;
	
	connection->sock = sock;
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

