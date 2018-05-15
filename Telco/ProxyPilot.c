#include <stdio.h>
#include <stdlib.h>

extern void setRobotVelocity(VelocityVector vel)
{
	sendMsgSetRobotVelocity(vel);
}

extern void askPilotState()
{
	sendMsgAskPilotState();
}

extern void toggleEmergencyStop()
{
	sendMsgToggleEmergencyStop();
}

