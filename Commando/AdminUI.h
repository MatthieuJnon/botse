#ifndef ADMIN_UI_H
#define
#include <stdint.h>
#include <sys/socket.h>

extern void start(void);
extern void stop(void);
extern void toggleEmergencyStop(void);
/*
 * AdminUI.h
 *
 *  Created on: 14 mai 2018
 *  Author: Pierre P
 */

extern void ask4Log(void);
extern void backMainScreen(void);
extern void clearLog(void);
extern void setDir(Direction dir);
extern void setEvents(Event event[]);
extern void setEventCount(Indice indice);

// STRUCTURES
typedef enum {FORGET=0, LEFT, RIGHT, FORWARD, BACKWARD} Direction;
typedef enum {NO_BUMP=0, BUMPED} Collision;

typedef struct
{
    int speed;
    int collision;
    float luminosity;
} PilotState;


typedef struct
{
    Direction dir;
    int power;
} VelocityVector;


typedef struct sockaddr_in {
    sa_family_t    sin_family; /* Family address : AF_INET */
    uint16_t       sin_port;   /* port byte network      */
    struct in_addr sin_addr;   /* Internet address         */
} Address;


struct in_addr {
    uint32_t       s_addr;     /* Address in network byte order */
};


#endif
