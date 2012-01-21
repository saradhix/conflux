
struct flux_connection
{
int sock;
};
struct flux_connection *flux_connection_init(int sock);
void flux_connection_cleanup(struct flux_connection *connection);
