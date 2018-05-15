#include <stdio.h>
#include <stdlib.h>
#include "prose.h"
#include "RemoteUI.h"
#include "Keyboard.h"

static int CAPTURE = 0;
static pthread_t myThread;

void * threadProc(void* param);

extern void KeyBoard_Start(){
    CAPTURE = 1;

    if(pthread_create(&myThread, NULL, &threadProc, NULL) == -1) {
        STOP_ON_ERROR(1);
    }

    AdminUI_start();

}

extern void KeyBoard_Stop(){
    CAPTURE = 0;

    pthread_join(myThread, NULL);

    AdminUI_stop();

}

void * threadProc(void* param){
    while(CAPTURE) {

        char choice;
        char escape;

        scanf("%c", &choice);
        scanf("%c", &escape);

        fprintf(stdout, "\n", choice);

        switch (choice) {

			case 'z':
				RemoteUI_setDir(FORWARD);
				break;

			case 's':
				RemoteUI_setDir(BACKWARD);
				break;

			case 'd':
				RemoteUI_setDir(RIGHT);
				break;

			case 'q':
				RemoteUI_setDir(LEFT);
				break;

            case 'a':
            	RemoteUI_backToMainScreen();
                break;

            case 'e':
                RemoteUI_clear();
                break;

            case 'r':
                RemoteUI_ask4log();
                break;

            case 'quit':
                RemoteUI_stop();
                break;

            case '\n':
                break;

            default:
            	break;

        }

    }
}
