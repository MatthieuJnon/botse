#ifndef POSTMAN_TELCO_H
#define POSTMAN_TELCO_H

#define MAX_PENDING_CONNECTIONS (2)
#define SERVER_PORT (12345)

int PostmanTelco_new();
extern void sendMsg(int socket, char *msg);
extern void *receiveMsg(void *param);
void PostmanTelco_free();

#endif
