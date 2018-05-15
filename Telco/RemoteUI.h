#ifndef REMOTE_UI_H
#define REMOTE_UI_H

typedef enum {
	SCREEN_FORGET,
    SCREEN_MAIN,
    SCREEN_LOG,
	SCRREN_DEATH,
	NB_SCREEN
} IdScreen;

extern void RemoteUI_start(void);
extern void RemoteUI_stop(void);
extern void RemoteUI_setIP(int ip);
extern void RemoteUI_setDir(Direction dir);
extern void RemoteUI_validate(void);
extern void RemoteUI_toggleEmergencyStop(void);
extern void RemoteUI_setPiloteState (PilotState state);
extern void RemoteUI_setEvents (Event events[]);
extern void RemoteUI_setEventCount(int indice);

#endif

