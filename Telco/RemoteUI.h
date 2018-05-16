#ifndef REMOTE_UI_H
#define REMOTE_UI_H

typedef enum {
	SCREEN_FORGET,
    SCREEN_MAIN,
    SCREEN_LOG,
	SCREEN_DEATH,
	NB_SCREEN
} IdScreen;

//typedef enum {LEFT=0, RIGHT, FORWARD, BACKWARD} Direction;

//type VelocityVector and PilotState already declared in ProxyPilot.h

/*
typedef struct
{
    Direction dir;
    int power;
} VelocityVector;
*/

/*
typedef struct
{
    int speed;
    int collision;
    float luminosity;
} PilotState;
*/

// On commence toujours par l'état S_FORGET, suivi de l'état initial de la MAE
#define STATE_GENERATION S(S_FORGET) S(S_MAIN_SCREEN) S(S_LOG_CREEN) S(S_DEATH)
#define S(x) x,
typedef enum {STATE_GENERATION STATE_NB} State;
#undef S
#define S(x) #x,
const char * const State_Name[] = { STATE_GENERATION };
#undef STATE_GENERATION
#undef S
static const char * State_getName(int i)
{
    return State_Name[i];
}

//On commence toujours par A_NOP : pas d'action à faire
#define ACTION_GENERATION S(A_NOP) S(A_SET_DIR) S(A_SET_IP) S(A_DISPLAY_LOG) S(A_RETURN) S(A_CLEAR) S(A_STOP) S(A_REFRESH)
#define S(x) x,
typedef enum {ACTION_GENERATION ACTION_NB}  Action;
#undef S
#define S(x) #x,
const char * const Action_Name[] = { ACTION_GENERATION };
#undef ACTION_GENERATION
#undef S

static const char * Action_getName(int i)
{
    return Action_Name[i];
}


#define EVENT_GENERATION S(E_SET_DIR) S(E_SET_IP) S(E_REFRESH) S(E_CLEAR) S(E_ASK_LOG) S(E_RETURN) S(E_STOP)
#define S(x) x,
typedef enum {EVENT_GENERATION EVENT_NB}  Event;
#undef S
#define S(x) #x,
const char * const EventName[] = { EVENT_GENERATION };
#undef EVENT_GENERATION
#undef S

static const char * Event_getName(int i)
{
    return EventName[i];
}


typedef struct
{
	State destinationState;
	Action action;
} Transition;

typedef struct
{
	Event event;
// définition générique pour permettre l'ajout d'attributs dans les messages si besoin
// Ajout des attributs si nécessaire pour les actions
} MqMsg;



extern void RemoteUI_new(void);
extern void RemoteUI_start(void);
extern void RemoteUI_stop(void);
extern void RemoteUI_setIP(uint8_t ip);
extern void RemoteUI_setDir(Direction dir);
extern void RemoteUI_backToMainScreen(void);
extern void RemoteUI_ask4log(void);
extern void RemoteUI_clear(void);
extern void RemoteUI_validate(void);
extern void RemoteUI_toggleEmergencyStop(void);
extern void RemoteUI_setPiloteState (PilotState state);
extern void RemoteUI_setEvents (Event events[]);
extern void RemoteUI_setEventCount(int indice);

//extern void RemoteUI_mqSend(MqMsg msg);

#endif
