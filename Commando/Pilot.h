#ifndef PILOT_H
#define PILOT_H

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

typedef struct Pilot_t Pilot;

extern void Pilot_start(void);
extern void Pilot_stop(void);
extern MqMsg Pilot_mqReceive(void);
extern Pilot_setRobotVelocity(VelocityVector vel);
extern void Pilot_toggleEmergencyStop(void);
extern void Pilot_check(void);
extern void Pilot_new(void);
extern void Pilot_free(void);

#endif

