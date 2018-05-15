#ifndef PROXY_PILOT_H
#define PROXY_PILOT_H


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

extern void setRobotVelocity(VelocityVector vel);
extern void askPilotState(void);
extern void toggleEmergencyStop(void);

#endif