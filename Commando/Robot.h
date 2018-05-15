#ifndef ROBOT_H
#define ROBOT_H

extern void setWheelsVelocity(WheelVelocity vr, WheelVelocity vl);
extern Speed getRobotSpeed(void);
extern SensorsState getSensorsState(void);
extern Boolean hasBumped(void);

#endif
