#include "prose.h"
#include "Robot.h"
#include "Pilot.h"

#ifndef ADMIN_UI_H
#define ADMIN_UI_H
#include <stdint.h>
#include <sys/socket.h>

// STRUCTURES
typedef struct AdminUI_t AdminUI;

typedef enum {
	SCREEN_FORGET,
    SCREEN_MAIN,
    SCREEN_LOG,
	SCREEN_DEATH,
	NB_SCREEN
} IdAdminScreen;

typedef struct
{
    int speed;
    int collision;
    float luminosity;
} AdminUIState;

typedef struct {
    uint32_t       s_addr;     /* Address in network byte order */
}in_addr;

typedef struct sockaddr_in {
    sa_family_t    sin_family; /* Family address : AF_INET */
    uint16_t       sin_port;   /* port byte network      */
    in_addr sin_addr;   /* Internet address         */
} Address;



extern void start(void);
extern void stop(void);
extern void toggleEmergencyStop(void);
extern void AdminUI_ask4Log(void);
extern void AdminUI_backMainScreen(void);
extern void AdminUI_clearLog(void);
extern void AdminUI_setDir(Direction dir);
extern void AdminUI_setEvents(Event event[]);
extern void AdminUI_setEventCount(int indice);

#endif
