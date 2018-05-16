#include "StarterT.h"
#include "DispatcherTelco.h"
#include "ProxyLogger.h"
#include "ProxyPilot.h"
#include "RemoteUI.h"
#include "PostmanTelco.h"

extern void start(void){
  PostmanTelco_new();
  RemoteUI_start();
  DT_start();
}

extern void stop(void){
  RemoteUI_stop();
  DT_stop();
  PostmanTelco_free();
}
