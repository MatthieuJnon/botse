#include <stdio.h>
#include <stdlib.h>

extern void askEvents(Indice from, Indice to)
{
	sendMsgAskEvent(from, to);
}

extern void askEventCount()
{
	sendMsgAskEvent();
}
