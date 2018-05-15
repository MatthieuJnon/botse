/*
 * AdminUI.c
 *
 *  Created on: 14 mai 2018
 *  Author: Pierre P
 */

#include <stdlib.h>
#include <stdio.h>
#include <printf.h>
#include <errno.h>

#include "AdminUI.h"

// VARIABLES
static IdScreen		 	myCurrentScreen;
/* static VelocityVector 	myVel; */
static Indice 			currentEventNumber;
static Indice 			previousEventNumber;
static Event			myEvents[];

// VARIABLES CREATED
static PilotState state;
static int testSocket;

// PROTOTYPES
static void displayScreen (IdScreen idAdminScreen);
static void displayEvents(Event events[]);

/**
 * start
 *
 * @brief The user asked to start
 * connection to socket.
 */
extern void adminui_start()
{
	int answer = 0;

	if (connect (testSocket, (struct sockaddr *) &Address, sizeof(Address)) == -1){
		perror("Connection");
		exit(errno);
	}

	if (read(testSocket, &answer, sizeof(answer) < 0)){
		perror("Read");
		exit(errno);
	}

	if (answer == 1){
		printf("Connection OK to server\n");
	}

	printf("Connected to Pilot\n");

	run();
}

/**
 * stop
 *
 * @brief The user asked to stop
 * break connection to socket.
 */
extern void adminui_stop()
{
	close(testSocket);

	printf("Stop connection to Pilot\n");
}

/**
 * ask4Log
 *
 * @brief The user asked to give the robot information
 * get the pilot information and prints it in the console.
 */
static void ask4Log()
{
	fprintf(stdout, "Robot status :\n");
    fprintf(stdout, "Current speed : %d\n", state.speed);
    fprintf(stdout, "Current Luminosity : %f\n", state.luminosity);

    if (state.collision == BUMPED) {
    	fprintf(stdout, "Bumper status : Obstacle found\n");
    }
    else {
    	fprintf(stdout, "Bumper status : No Obstacle\n");
    }
}

/**
 * clearLog
 *
 * @brief The user asked to clear the console flux
 */
extern void clearLog()
{
	system("clear");
}

/**
 * setDir
 *
 * @brief The user asked to move
 * Pilot functions to move in the appropriate direction.
 * @param dir -> the direction to go.
 */
extern void setDir(Direction dir)
{
	VelocityVector myvel;

    printf("Direction : %d\n", dir);

    if (dir == FORWARD){
    	if (state.speed + 5 > 100){
			fprintf(stdout, "Max speed attained\n");
			myvel.power = state.speed;
    	}
    	else{
    		myvel.power = state.speed + 5;
    	}
    }
    else if (dir == BACKWARD){
    	if (state.speed - 5 < -100){
    		fprintf(stdout, "Max reverse speed attained\n");
    		myvel.power = state.speed;
    	}
    	else{
    		myvel.power = state.speed - 5;
    	}
    }
    else {
    	myvel.power = state.speed;
    }
    myvel.dir = dir;
}


/* ============================================
 * 	A REVOIR
 * ============================================
 */

extern void toggleEmergencyStop()
{

}

extern void backMainScreen()
{

}

extern void setEvents(Event event[])
{

}

extern void setEventCount(Indice indice)
{

}

static void displayScreen (IdScreen idAdminScreen)
{
	fprintf(stdout, "Id admin : %d\n", idAdminScreen);
}

static void displayEvents(Event events[])
{

}
