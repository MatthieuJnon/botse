#include "prose.h"

#ifndef POSTMAN_COMMANDO_H
#define POSTMAN_COMMANDO_H

#define PORT 12345

typedef struct {
	Indice indice;
	PiloteState PiloteState;
	Event events;
} DataSend;

typedef struct{
	char[200] dataType;
	Data data;
} SocketDataSend;


extern int PostmanCommando_new(void);

extern void sendMsg();
extern void sendMsgSetPilotState(PilotState pilotState);
extern void sendMsgSetEvents(Event event);
extern void sendMsgSetEventCount(Indice indice);

extern void *receiveMsg(void *params);
extern void PostmanCommando_free(void);

#endif
