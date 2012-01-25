int send_connect_msg(int fd, char *id);
int send_publish_msg(int fd, char *topic, void *payload, int payload_length);
int send_subscribe_msg(int fd, char *topic);

