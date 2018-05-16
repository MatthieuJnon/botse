#include <stdio.h>
#include <stdlib.h>

#include "ProxyPilot.h"
#include "ProxyLogger.h"
#include "PostmanTelco.h"

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
