#include "StarterC.h"
#include "Pilot.h"
#include "AdminUI.h"
#include "DispatcherCommando.h"
#include "Robot.h"
#include "Logger.h"
#include "ProxyRemoteUI.h"
#include "PostmanCommando.h"

extern void start(void) {
    adminui_start();
    Pilot_start();
    Logger_start();
    DC_start();
}

extern void stop(void) {
    Logger_stop();
    Pilot_stop();
    adminui_stop();
    DC_stop();
}
