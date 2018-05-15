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
#include "ProxyPilot.c"

// PARAMETRE MAE
#define NAME_MQ_BOX  "/bal_AdminUI"
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

#define ACTION_GENERATION S(A_NOP) S(A_SET_DIR) S(A_DISPLAY_LOG) S(A_RETURN) S(A_CLEAR) S(A_STOP) S(A_REFRESH)
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


#define EVENT_GENERATION S(E_SET_DIR) S(E_REFRESH) S(E_CLEAR) S(E_ASK_LOG) S(E_RETURN) S(E_STOP)
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

// Attributs de AdminUI (singleton)
static State MyState;
static pthread_t MyThread;

/* MAE de AdminUI */
static Transition MySm [STATE_NB][EVENT_NB] =
{
	    [S_MAIN_SCREEN][E_SET_DIR] = {S_MAIN_SCREEN, A_SET_DIR},
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
static Direction myDir;
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
}

extern void AdminUI_stop(void)
{

	MqMsg msg = {.event = E_STOP};
	TRACE("[AdminUI] demande d'arret de la lampe  \n");
	AdminUI_mqSend (msg);

	pthread_join(MyThread, NULL);

	AdminUI_free();

}

extern void AdminUI_setDir(Direction dir)
{
	myDir = dir;
}

extern void AdminUI_validate(void)
{

}

extern void AdminUI_toggleEmergencyStop(void)
{
	//ProxyPilot.toggleEmergencyStop();
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
			printf "-----------------------Screen MAIN-------------------------------";
			break;
		case SCREEN_LOG:
			printf "-----------------------Screen LOG--------------------------------";
			break;
		default:
			break;
	}
}

static AdminUI_displayEvents(Event event[])
{
	//TODO
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
	/* envoi de l'ordre à travers la BAL */
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

	/* Create BAL */
	mqa.mq_maxmsg = MQ_MSG_COUNT;
	mqa.mq_msgsize = MQ_MSG_SIZE;
	mqa.mq_flags = 0;

	check = mq_unlink (NAME_MQ_BOX);
	mq = mq_open (NAME_MQ_BOX, O_CREAT, 0777, &mqa);
	STOP_ON_ERROR(mq == -1);

	check = mq_close (mq);
	STOP_ON_ERROR(check != 0);
 	MyState = S_MAIN_SCREEN;
	check = pthread_create (&MyThread, NULL, AdminUI_run, NULL);
	STOP_ON_ERROR(check != 0);
}

extern void RemoteUI_free ()
{
	int check;
	/* Destruction BAL */
	check = mq_unlink (NAME_MQ_BOX);
	STOP_ON_ERROR(check != 0);
	TRACE("[AdminUI] Destruction \n");
}

static void AdminUI_performAction (Action anAction)
{
	switch (anAction)
	{
    case A_SET_DIR:
        AdminUI_setDir();
        break;

    case A_DISPLAY_LOG:
    	displayScreen(SCREEN_LOG);
    	AdminUI_displayEvents(myEvents);
        break;

    case A_RETURN:
    	AdminUI_displayScreen(SCREEN_MAIN);
    	break;

    case A_CLEAR:
    	AdminUI_displayScreen(SCREEN_LOG);
        break;

    case A_STOP:
    	AdminUI_stop();
        break;

    case A_REFRESH:
    	currentEventNumber=ProxyLogger_askEventsCount();
    	myEvents=getEvents(previousEventNumber, currentEventNumber);
    	previousEventNumber = currentEventNumber;
    	AdminUI_displayEvents(myEvents);
    	break;

    default:
        TRACE("[AdminUI] Action inconnue, problème dans la MAE \n ");
        break;
	}
}

static void* AdminUI_run ()
{
	MqMsg msg;
	Action act;

	while (MyState != S_DEATH)
	{
		msg = AdminUI_mqReceive ();
		if (MySm[MyState][msg.event].destinationState == S_FORGET)
		{
			TRACE("[AdminUI] MAE, perte evenement %s  \n", Event_getName(msg.event));
		}
		else
		{
			TRACE("[AdminUI] MAE, traitement evenement %s \n",  Event_getName(msg.event));
			act = MySm[MyState][msg.event].action;
			TRACE("[AdminUI] MAE, traitement action %s \n", Action_getName(act));
			AdminUI_performAction (act);
			MyState = MySm[MyState][msg.event].destinationState;
            TRACE("[AdminUI] MAE, va dans etat %s \n", State_getName(MyState));
		}
	}
	return (0);
}
