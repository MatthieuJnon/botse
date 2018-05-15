#ifndef REMOTE_UI_H
#define REMOTE_UI_H

typedef enum {
	SCREEN_FORGET,
    SCREEN_MAIN,
    SCREEN_LOG,
	SCREEN_DEATH,
	NB_SCREEN
} IdScreen;

typedef enum {LEFT=0, RIGHT, FORWARD, BACKWARD} Direction;

typedef struct
{
    Direction dir;
    int power;
} VelocityVector;

typedef struct
{
    int speed;
    int collision;
    float luminosity;
} PilotState;

extern void RemoteUI_start(void);
extern void RemoteUI_stop(void);
extern void RemoteUI_setIP(int ip);
extern void RemoteUI_setDir(Direction dir);
extern void RemoteUI_backToMainScreen(void);
extern void RemoteUI_ask4log(void);
extern void RemoteUI_clear(void);
extern void RemoteUI_validate(void);
extern void RemoteUI_toggleEmergencyStop(void);
extern void RemoteUI_setPiloteState (PilotState state);
extern void RemoteUI_setEvents (Event events[]);
extern void RemoteUI_setEventCount(int indice);

#endif

