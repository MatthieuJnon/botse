/*
 * AdminUI.c
 *
 *  Created on: 14 mai 2018
 *  Author: Pierre P
 */
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <mqueue.h>
#include <stdbool.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include "AdminUI.h"

// PARAMAMETRE MAE
#define NAME_MQ_BOX  "/bal_RemoteUI"
#define MQ_MSG_COUNT 10
#define MQ_MSG_SIZE 256

#define STATE_GENERATION S(S_FORGET) S(S_MAIN_SCREEN) S(S_LOG_CREEN) S(S_DEATH)
#define S(x) x,
typedef enum {STATE_GENERATION STATE_NB} State;
#undef S
#define S(x) #x,
const char * const State_Name[] = { STATE_GENERATION };
#undef STATE_GENERATION
#undef S
static const char * State_getName(int i)
{
    return State_Name[i];
}

#define ACTION_GENERATION S(A_NOP) S(A_SET_DIR) S(A_SET_IP) S(A_DISPLAY_LOG) S(A_RETURN) S(A_CLEAR) S(A_STOP) S(A_REFRESH)
#define S(x) x,
typedef enum {ACTION_GENERATION ACTION_NB}  Action;
#undef S
#define S(x) #x,
const char * const Action_Name[] = { ACTION_GENERATION };
#undef ACTION_GENERATION
#undef S
static const char * Action_getName(int i)
{
    return Action_Name[i];
}

#define EVENT_GENERATION S(E_SET_DIR) S(E_SET_IP) S(E_REFRESH) S(E_CLEAR) S(E_ASK_LOG) S(E_RETURN) S(E_STOP)
#define S(x) x,
typedef enum {EVENT_GENERATION EVENT_NB}  Event;
#undef S
#define S(x) #x,
const char * const EventName[] = { EVENT_GENERATION };
#undef EVENT_GENERATION
#undef S
static const char * Event_getName(int i)
{
    return EventName[i];
}

typedef struct
{
	State destinationState;
	Action action;
} Transition;

typedef struct
{
	Event event;
} MqMsg;

static State MyState;
static pthread_t MyThread;

static Transition MySm [STATE_NB][EVENT_NB] =
{
	    [S_MAIN_SCREEN][E_SET_DIR] = {S_MAIN_SCREEN, A_SET_DIR},
	    [S_MAIN_SCREEN][E_SET_IP]  = {S_MAIN_SCREEN, A_SET_IP},
	    [S_MAIN_SCREEN][E_ASK_LOG] = {S_LOG_CREEN,   A_DISPLAY_LOG},
	    [S_MAIN_SCREEN][E_STOP]    = {S_DEATH,       A_STOP},

	    [S_LOG_CREEN][E_RETURN]  = {S_MAIN_SCREEN, A_RETURN},
	    [S_LOG_CREEN][E_CLEAR]   = {S_LOG_CREEN,   A_CLEAR},
	    [S_LOG_CREEN][E_REFRESH] = {S_LOG_CREEN,   A_REFRESH},
	    [S_LOG_CREEN][E_STOP]    = {S_DEATH,       A_STOP},
};

typedef union
{
	char buffer[MQ_MSG_SIZE];
	MqMsg data;
} MqMsgAdapter;

// VARIABLES
static uint8_t myIp;
static Direction myDir;
static VelocityVector myVel;
static Event myEvents[];
static int currentEventNumber;
static int previousEventNumber;
static int nbEvents;

// PROTOTYPES
static AdminUI_displayScreen(IdAdminScreen idAdminScreen);
static AdminUI_displayEvents(Event event[]);

extern void AdminUI_start(void)
{
	previousEventNumber = 0;
	AdminUI_new ();
	KeyBoard_Start();
}

extern void AdminUI_stop(void)
{
	MqMsg msg = {.event = E_STOP};
	TRACE("[AdminUI] demande d'arret de AdminUI \n");
	AdminUI_mqSend (msg);

	pthread_join(MyThread, NULL);

	AdminUI_free();
	KeyBoard_Stop();
}

extern void AdminUI_backToMainScreen(void){
	MqMsg msg =  {.event = E_RETURN};
	TRACE("[AdminUI] retour a l'ecran principal\n");
	AdminUI_mqSend (msg);
}

extern void AdminUI_ask4log(void){
	MqMsg msg =  {.event = E_ASK_LOG};
	TRACE("[RemoteUI] demande  de Log\n");
	AdminUI_mqSend (msg);
}

extern void AdminUI_clear(void){
	MqMsg msg =  {.event = E_CLEAR};
	TRACE("[AdminUI] clear logs\n");
	AdminUI_mqSend (msg);
}

extern void AdminUI_setDir(Direction dir)
{
	myDir = dir;
	myVel.power = (myDir==FORWARD)? 20 : -20;
	myVel.dir = myDir;
	MqMsg msg =  {.event = E_SET_DIR};
	TRACE("[AdminUI] demande de changement du dir\n");
	AdminUI_mqSend (msg);
}

extern void AdminUI_validate(void)
{

}

extern void AdminUI_toggleEmergencyStop(void)
{

}

extern void AdminUI_setEvents (Event events[])
{
	for(int ind=0; ind<nbEvents; ind++){
		myEvents[ind] = events[ind];
	}
}

extern void AdminUI_setEventCount(int indice)
{
	nbEvents = indice;
}

static AdminUI_displayScreen(IdAdminScreen idAdminScreen)
{
	switch(idAdminScreen){
		case SCREEN_MAIN:
			printf("\n-----------------------Screen Main--------------------------------\n");
			break;
		case SCREEN_LOG:
			printf("\n-----------------------Screen lOG  --------------------------------\n");
			break;
		case SCREEN_DEATH:
			printf("\n--------------------- Shutting Down  ------------------------------\n");
			break;
		default:
			break;
	}
}

static AdminUI_displayEvents(Event event[])
{

}

MqMsg AdminUI_mqReceive ()
{
	int check;
	mqd_t mq;
	MqMsgAdapter msg;
	mq = mq_open (NAME_MQ_BOX, O_RDONLY);
	STOP_ON_ERROR(mq == -1);
	check = mq_receive (mq, msg.buffer, MQ_MSG_SIZE, 0);
	STOP_ON_ERROR(check != sizeof (msg.buffer));
	check = mq_close (mq);
	STOP_ON_ERROR(check != 0);
	return msg.data;
}

static void AdminUI_mqSend (MqMsg aMsg)
{
	int check;
	MqMsgAdapter msg;
	mqd_t mq;
	msg.data = aMsg;
	mq = mq_open (NAME_MQ_BOX, O_WRONLY);
	STOP_ON_ERROR(mq == -1);
	check = mq_send (mq, msg.buffer, sizeof (msg.buffer), 0);
	STOP_ON_ERROR(check != 0);
	check = mq_close (mq);
	STOP_ON_ERROR(check != 0);
}


extern void AdminUI_new ()
{
	struct mq_attr mqa;
	int check;
	mqd_t mq;
    TRACE("[AdminUI] Construction \n");

	/* creation de la BAL */
	mqa.mq_maxmsg = MQ_MSG_COUNT;
	mqa.mq_msgsize = MQ_MSG_SIZE;
	mqa.mq_flags = 0;
    check = mq_unlink (NAME_MQ_BOX);
	mq = mq_open (NAME_MQ_BOX, O_CREAT, 0777, &mqa);
	STOP_ON_ERROR(mq == -1);

	check = mq_close (mq);
	STOP_ON_ERROR(check != 0);
 	MyState = S_MAIN_SCREEN;
	check = pthread_create (&MyThread, NULL, RemoteUI_run, NULL);
	STOP_ON_ERROR(check != 0);
}

extern void AdminUI_free ()
{
	int check;
	/* destruction de la BAL */
	check = mq_unlink (NAME_MQ_BOX);
	STOP_ON_ERROR(check != 0);
	TRACE("[AdminUI] Destruction \n");
}

static void AdminUI_performAction (Action anAction)
{
	switch (anAction)
	{
    case A_SET_DIR:
    	AdminUI_displayScreen(SCREEN_MAIN);
        break;

    case A_DISPLAY_LOG:
    	AdminUI_displayScreen(SCREEN_LOG);
//      AdminUI_displayEvents(myEvents); ??
        break;

    case A_RETURN:
    	AdminUI_displayScreen(SCREEN_MAIN);
    	break;

    case A_CLEAR:
//    	AdminUI_clear ();
    	AdminUI_displayScreen(SCREEN_LOG);
        break;

    case A_STOP:
    	AdminUI_displayScreen(SCREEN_DEATH);
    	break;

    case A_REFRESH:
    	currentEventNumber=ProxyLogger_askEventsCount();
    	myEvents=getEvents(previousEventNumber, currentEventNumber);
    	previousEventNumber = currentEventNumber;
//    	AdminUI_displayEvents(myEvents); ??
    	AdminUI_displayScreen(SCREEN_LOG);
    	break;

    default:
        TRACE("[AdminUI] Action inconnue, pb ds la MAE \n ");
        break;
	}
}


static void* RemoteUI_run ()
{
	MqMsg msg;
	Action act;

	while (MyState != S_DEATH)
	{
		msg = Lampe_mqReceive ();
		if (MySm[MyState][msg.event].destinationState == S_FORGET) // aucun état de destination, on ne fait rien
		{
            // on trace juste l'évènement détruit
			TRACE("[RemoteUI] MAE, perte evenement %s  \n", Event_getName(msg.event));
		}
		else /* on tire la transition */
		{
			TRACE("[RemoteUI] MAE, traitement evenement %s \n",  Event_getName(msg.event));
			act = MySm[MyState][msg.event].action;
			TRACE("[RemoteUI] MAE, traitement action %s \n", Action_getName(act));
			RemoteUI_performAction (act);
			MyState = MySm[MyState][msg.event].destinationState;
            TRACE("[RemoteUI] MAE, va dans etat %s \n", State_getName(MyState));
		}
	}
	return (0);
}
