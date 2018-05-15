#ifndef POSTMAN_COMMANDO_H
#define POSTMAN_COMMANDO_H

#define PORT 12345

typedef struct{
	PilotState PilotState;
	Event[] events;
	Indice indice;
} SocketData;

int PostmanCommando_new(void);
extern void sendMsg();
extern void *receiveMsg(void *params);
void PostmanCommando_free(void);

#endif
