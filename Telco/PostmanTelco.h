#ifndef POSTMAN_TELCO_H
#define POSTMAN_TELCO_H

#define MAX_PENDING_CONNECTIONS (2)
#define SERVER_PORT (12345)

typedef struct{
	PilotState PilotState;
	Event[] events;
	Indice indice;
} SocketData;

int PostmanTelco_new();
extern void sendMsg(int socket, char *msg);
extern void *receiveMsg(void *param);
void PostmanTelco_free();

#endif
