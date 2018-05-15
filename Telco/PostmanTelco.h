#ifndef POSTMAN_TELCO_H
#define POSTMAN_TELCO_H

#define MAX_PENDING_CONNECTIONS (2)
#define PORT (12345)

typedef struct {
	Indice fromIndice;
	Indice toIndice;
	VelocityVector velocityVector;
} Data;

typedef struct{
	char[200] dataType;
	Data data;
} SocketData;

extern int PostmanTelco_new();

extern void sendMsg(SocketData socketData);
extern void sendMsgAskEvent(Indice from, Indice to);
extern void sendMsgAskEventCount();
extern void sendMsgSetRobotVelocity(Velocity vel);
extern void sendMsgAskPilotState();
extern void sendMsgToggleEmergencyStop();

extern void *receiveMsg(void *param);
extern void PostmanTelco_free();

#endif
