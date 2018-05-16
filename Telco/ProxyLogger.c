#include <stdio.h>
#include <stdlib.h>

#include "ProxyLogger.h"

static int from;
static int to;

extern void askEvents(from, to)
{
	sendMsgAskEvent(from, to);
}

extern void askEventCount()
{
	sendMsgAskEvent();
}
