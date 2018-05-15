#ifndef POSTMAN_TELCO_H
#define POSTMAN_TELCO_H

#define MAX_PENDING_CONNECTIONS (2)
#define PORT (12345)

typedef enum {
	ASK_EVENT,
	ASK_EVENT_COUNT,
	ASK_PILOT_STATE,
	SET_ROBOT_VELOCITY,
	TOGGLE_EMERGENCY_STOP
} DataTypeSend;

typedef struct {
	Indice fromIndice;
	Indice toIndice;
	VelocityVector velocityVector;
} DataSend;

typedef struct{
	DataTypeSend dataType;
	DataSend data;
} SocketDataSend;

typedef enum {
	TEST
} DataTypeReceive;

typedef struct {
	Indice fromIndice;
	Indice toIndice;
	VelocityVector velocityVector;
} DataReceive;

typedef struct{
	DataTypeReceive dataType;
	DataReceive data;
} SocketDataReceive;

extern int PostmanTelco_new();

extern void sendMsg(SocketDataSend socketData);
extern void sendMsgAskEvent(Indice from, Indice to);
extern void sendMsgAskEventCount();
extern void sendMsgSetRobotVelocity(VelocityVector vel);
extern void sendMsgAskPilotState();
extern void sendMsgToggleEmergencyStop();

extern void *receiveMsg(void *param);
extern void PostmanTelco_free();

#endif
