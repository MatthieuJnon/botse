#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <mqueue.h>
#include <stdbool.h>
#include <sys/time.h>
#include <sys/stat.h>        /* For mode constants */
#include <signal.h>
#include <errno.h>
#include <time.h>

#include "Pilot.h"
#include "Robot.h"

/*
 * paramètres de configuration du pilote
 */
#define NAME_MQ_BOX  "/bal_Pilot" //ne pas oublier le / devant le nom !
#define MQ_MSG_COUNT 10 // min et max variable pour Linux (suivant version noyau, cf doc)
#define MQ_MSG_SIZE 256 // 128 est le minimum absolu pour Linux, 1024 une valeur souvent utilisée !
#define TO_BUMP 1000000

//STRUCTURES
#define STATE_GENERATION S(S_FORGET) S(S_IDLE) S(S_RUNNING) S(S_TEST_BUMP) S(S_EMERGENCY) S(S_DEATH)
#define S(x) x,
typedef enum {STATE_GENERATION STATE_NB} State;
#undef S
#define S(x) #x,
const char * const State_Name[] = { STATE_GENERATION };
#undef STATE_GENERATION
#undef S

#define ACTION_GENERATION S(A_NOP) S(A_SEND_MVT) S(A_EVAL_BUMP) S(A_STOP) S(A_RESET_TO) S(A_SET_TO)
#define S(x) x,
typedef enum {ACTION_GENERATION ACTION_NB}  Action;
#undef S
#define S(x) #x,
const char * const Action_Name[] = { ACTION_GENERATION };
#undef ACTION_GENERATION
#undef S

#define EVENT_GENERATION S(E_EVENT_STOPPED) S(E_TO_BUMP) S(E_STOP) S(E_BUMPED) S(E_NOT_BUMPED) S(E_VELOCITY) S(E_TOGGLE_ES)
#define S(x) x,
typedef enum {EVENT_GENERATION EVENT_NB}  Event;
#undef S
#define S(x) #x,
const char * const EventName[] = { EVENT_GENERATION };
#undef EVENT_GENERATION
#undef S

typedef struct
{
	State destinationState;
	Action action;
} Transition;

struct Pilot_t
{
    Robot robot;
    VelocityVector currentVel;
    PilotState myPilotState;
	State state;
};

typedef struct
{
	Event event;
} MqMsg;

typedef union
{
	char buffer[MQ_MSG_SIZE];
	MqMsg data;
} MqMsgAdapter;

typedef void (*ActionPtr)();

// PROTOTYPES
static void Pilot_mqSend (MqMsg aMsg);
static void* Pilot_run();
static void Pilot_sendMvt();
static int evaluateVel(VelocityVector vel);
static void Pilot_setTO(void);
static void Pilot_resetTO(void);
static void* Pilot_timeout(void);
static MqMsg Pilot_mqReceive(void);

// VARIABLES
static Pilot myPilot;
static Transition mySm[STATE_NB][EVENT_NB] =
{
	[S_IDLE][E_VELOCITY]={S_RUNNING,A_SEND_MVT},
	[S_IDLE][E_STOP]={S_DEATH,A_STOP},
	[S_IDLE][E_TOGGLE_ES]={S_EMERGENCY,A_SEND_MVT},
	[S_EMERGENCY][E_TOGGLE_ES]={S_IDLE,A_NOP},
	[S_EMERGENCY][E_STOP]={S_DEATH,A_STOP},
	[S_TEST_BUMP][E_TOGGLE_ES]={S_EMERGENCY,A_SEND_MVT},
	[S_TEST_BUMP][E_BUMPED]={S_IDLE,A_SEND_MVT},
	[S_TEST_BUMP][E_NOT_BUMPED]={S_RUNNING,A_SET_TO},
	[S_RUNNING][E_EVENT_STOPPED]={S_IDLE,A_RESET_TO},
	[S_RUNNING][E_VELOCITY]={S_RUNNING,A_SEND_MVT},
	[S_RUNNING][E_TO_BUMP]={S_TEST_BUMP,A_EVAL_BUMP},
	[S_RUNNING][E_STOP]={S_DEATH,A_STOP},
	[S_RUNNING][E_TOGGLE_ES]={S_EMERGENCY,A_RESET_TO}
};
static pthread_t mainThread, timeoutThread;
static const ActionPtr ActionsTab[ACTION_NB] = {&Pilot_stop, &Pilot_sendMvt, &Pilot_toggleEmergencyStop, &Pilot_check, &Pilot_setTO, &Pilot_resetTO};
static int timeout_counter;

// FONCTIONS
extern void Pilot_start()
{
	myPilot.currentVel.dir = FORWARD;
	myPilot.currentVel.power = 0;
	myPilot.state = S_IDLE;
	Pilot_sendMvt(myPilot.currentVel.power);
}

extern void Pilot_stop()
{
	MqMsg msg = {.event = E_STOP};

	myPilot.currentVel.dir = FORWARD;
	myPilot.currentVel.power = 0;

	Pilot_mqSend (msg);
}

//extern MqMsg Pilot_mqReceive ()
static MqMsg Pilot_mqReceive ()
{
	int check;
	mqd_t mq;
	MqMsgAdapter msg;
	mq = mq_open (NAME_MQ_BOX, O_RDONLY);

	check = mq_receive (mq, msg.buffer, MQ_MSG_SIZE, 0);
	if(check== -1){
		//gestion d'erreur
	}

	check = mq_close (mq);
	if(check== -1){
		//gestion d'erreur
	}

	return msg.data;
}

static void Pilot_mqSend (MqMsg aMsg)
{
	int check;
	MqMsgAdapter msg;
	mqd_t mq;
	msg.data = aMsg;
	/* envoi de l'ordre à travers la BAL */
	mq = mq_open (NAME_MQ_BOX, O_WRONLY);

	check = mq_send (mq, msg.buffer, sizeof (msg.buffer), 0);
	if(check== -1){
		//gestion d'erreur
	}

	check = mq_close (mq);
	if(check== -1){
		//gestion d'erreur
	}

}

extern PilotState Pilot_getState(){
	return myPilot.myPilotState;
}

extern VelocityVector Pilot_getVelocity(){
	return myPilot.currentVel;
}

static int evaluateVel(VelocityVector vel)
{
	return vel.power !=0;
}

static void Pilot_setTO()
{
	timeout_counter = TO_BUMP;
}

static void Pilot_resetTO()
{
	timeout_counter = TO_BUMP;
}

extern void Pilot_setRobotVelocity(VelocityVector vel)
{
	MqMsg msg = {.event = E_VELOCITY};
	myPilot.currentVel = vel;
	Pilot_sendMvt(myPilot.currentVel);
	if(!evaluateVel(vel))
		setTO(1);
	Pilot_mqSend (msg);
}

extern void Pilot_toggleEmergencyStop()
{
	MqMsg msg = {.event = E_TOGGLE_ES};
	myPilot.currentVel.dir = FORWARD;
	myPilot.currentVel.power = 0;
	Pilot_sendMvt(myPilot.currentVel);
	Pilot_mqSend (msg);
}

extern void Pilot_hasBumped()
{
//	timeout_counter = Pilot_setTO();
	Pilot_setTO();
	if(Robot_hasBumped()){
		MqMsg msg = {.event = E_BUMPED};
		Pilot_mqSend (msg);
	}else{
		MqMsg msg = {.event = E_NOT_BUMPED};
		VelocityVector vel;
		vel.dir 	= FORWARD;
		vel.power 	= 0;
		Pilot_setRobotVelocity(vel);
		Pilot_sendMvt();
		Pilot_mqSend (msg);
	}
}

static void* Pilot_run()
{
	MqMsg msg;
	Action act;

	while (myPilot.state != S_DEATH)
	{
		msg = Pilot_mqReceive ();
		if (mySm[myPilot.state][msg.event].destinationState != S_FORGET)
		{
			act = mySm[myPilot.state][msg.event].action;
			ActionsTab[act]();
			myPilot.state = mySm[myPilot.state][msg.event].destinationState;
		}
	}

	Robot_free();
	Pilot_free ();
	return (0);
}

static void* Pilot_timeout()
{
	while (myPilot.state != S_DEATH)
	{
		usleep(timeout_counter);
		if(myPilot.state == S_TEST_BUMP || myPilot.state == S_RUNNING){
			MqMsg msg = {.event = E_TO_BUMP};
			Pilot_mqSend (msg);
		}
	}
	return (0);
}

static void Pilot_sendMvt()
{
	switch (myPilot.currentVel.dir){
			case LEFT:
				Robot_setWheelsVelocity(-myPilot.currentVel.power,myPilot.currentVel.power);
				break;

			case RIGHT:
				Robot_setWheelsVelocity(myPilot.currentVel.power,-myPilot.currentVel.power);
				break;

			case FORWARD:
				Robot_setWheelsVelocity(myPilot.currentVel.power,myPilot.currentVel.power);
				break;

			case BACKWARD:
                if(Robot_getRobotSpeed() !=0){
                    Robot_setWheelsVelocity(0,0);
                }else{
                    Robot_setWheelsVelocity(-myPilot.currentVel.power,-myPilot.currentVel.power);
                }
				break;

			default:
				Robot_setWheelsVelocity(0,0);
		}
}

extern void Pilot_new()
{
    struct mq_attr mqa;
	int check;
	mqd_t mq;

	/* creation de la BAL */
	mqa.mq_maxmsg = MQ_MSG_COUNT;
	mqa.mq_msgsize = MQ_MSG_SIZE;
	mqa.mq_flags = 0;
	// Destruction de la BAL au cas ou (si BAL mal détruite lors d'une précédente utilisation)
	check = mq_unlink (NAME_MQ_BOX);
	mq = mq_open (NAME_MQ_BOX, O_CREAT, 0777, &mqa);

	check = mq_close (mq);
	myPilot.state = S_IDLE;
	Robot_new();
	check = pthread_create (&mainThread, NULL, Pilot_run, NULL);
	check = pthread_create (&timeoutThread, NULL, Pilot_timeout, NULL);
}

extern void Pilot_free()
{
	int check;
	/* destruction de la BAL */
	check = mq_unlink (NAME_MQ_BOX);
}
