#ifndef ADMIN_UI_H
#define

extern void start(void);
extern void stop(void);
extern void toggleEmergencyStop(void);
extern void ask4Log(void);
extern void backMainScreen(void);
extern void clearLog(void);
extern void setDir(Direction dir);
extern void setEvents(Event event[]);
extern void setEventCount(Indice indice);

#endif
