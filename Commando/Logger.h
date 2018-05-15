
#include "prose.h"
#include "Robot.h"

#ifndef LOGGER_H
#define LOGGER_H

/**
 *\bref structure pour enregistrer tout les changement fait au robot
 */
typedef struct
{
	SensorState sensorState;
    float speed;
} Event;

extern void Logger_start(void);
extern void Logger_stop(void);
extern Event* askEvents(int from, int to);
extern int askEventCount(void);
extern void clear(void);

#endif //LOGGER_H
