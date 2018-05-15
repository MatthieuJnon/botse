#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "prose.h"
#include "Logger.h"

// MACROS
#define LIMIT_EVENT 1000

//ENUM
static enum {
	EMPTY = 0,
	LOADED,
	FULL
}ListStatus;

static enum{
	STOP=0,
	KEEP_GOING
}StopOrder;

// VARIABLES
static Event myEvents[LIMIT_EVENT];
static int index;
static ListStatus listStatus;
static StopOrder stopOrder;
pthread_t threadId;

// PROTOTYPES
static void appendEvent(SensorState sensors, int spd);

extern void Logger_start(void)
{
	index = 0;
	listStatus = EMPTY;
	stopOrder = KEEP_GOING;

	int returnCode;


	if(pthread_create(&threadId, NULL, &threadproc, NULL) == -1) {
		//TODO : Gestion erreur
	}

}

extern void Logger_stop(void)
{
	//On ordonne l'arret
	stopOrder = STOP;

	pthread_join(threadId, NULL);

	//et on vide la liste
	clear();
}

extern Event* askEvents(int from, int to)
{
	int size = to - from;
	Event listEvents[size];

	if((listStatus!=FULL)&&(to<index || from<index)){
		return NULL;
	}

	for(int ind=0; ind<size; ind++){
		if(listStatus==FULL){
			if((from+ind+index)>(LIMIT_EVENT-1)){
				listEvents[ind]= myEvents[((from+ind+index)-(LIMIT_EVENT-1))];
			}else{
				listEvents[ind]=myEvents[(from+ind+index)];
			}
		}else{
			listEvents[ind] = myEvents[from+ind];
		}
	}

	return listEvents;

}

extern int askEventCount(void)
{
	return (listStatus==FULL)?LIMIT_EVENT:index;
}

extern void clear(void)
{
	listStatus = EMPTY;
	index = 0;
}

static void appendEvent(SensorState sensors, int spd)
{
	if(index == LIMIT_EVENT-1){
		listStatus = FULL;
		index = 0;
	}

	static Event newEvent;
	newEvent.sensorState = sensors;
	newEvent.speed = spd;

	myEvents[index] = newEvent;

	listStatus = (listStatus==FULL)?FULL:LOADED;
}

static void *threadproc(void *arg)
{

	while (stopOrder == KEEP_GOING) { // tant qu'on a pas l'ordre de s'arreter

		sleep(1);
		usleep(250000); //250 ms
	    appendEvent(Robot_getSensorState(),  Robot_getRobotSpeed());

	}
    return 0;
}

