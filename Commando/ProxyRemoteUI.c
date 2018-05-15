#include <stdio.h>
#include <stdlib.h>

#include "Pilot.h"
#include "Logger.h"
#include "ProxyRemoteUI.h"

extern void setPilotState(PilotState pilotState)
{
	sendMsgSetPilotState(pilotState);
}

extern void setEvents(Event event)
{
	sendMsgSetEvents(event);
}

extern void setEventCount(Indice indice)
{
	sendMsgSetEventCount(indice);
}