#ifndef REMOTE_UI_H
#define REMOTE_UI_H

extern void start(void);
extern void stop(void);
extern void setIP(Ip ip);
extern void setDir(Direction dir);
extern void validate(void);
extern void toggleEmergencyStop(void);
extern void setPiloteState (PilotState state);
extern void setEvents (Event events[]);
extern void setEventCount(Indice indice);

#endif

