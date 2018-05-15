#include <stdio.h>
#include <stdlib.h>
#include "prose.h"
#include "Pilot.h"
#include "Keyboard.h"

static int CAPTURE = 0;
static pthread_t myThread;

void * threadProc(void* param);

extern void KeyBoard_Start(){
    CAPTURE = 1;

    if(pthread_create(&myThread, NULL, &threadProc, NULL) == -1) {
        perror("pthread_create"); //Gestion erreur
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
				AdminUI_setDir(FORWARD);
				break;

			case 's':
				AdminUI_setDir(BACKWARD);
				break;

			case 'd':
				AdminUI_setDir(RIGHT);
				break;

			case 'q':
				AdminUI_setDir(LEFT);
				break;

            case 'a':
//                AdminUI_backMainScreen();
                break;

            case 'e':
                AdminUI_clearLog();
                break;

            case 'r':
                AdminUI_ask4Log();
                break;

            case '\n':
                break;

            default: //This doesn't work for the moment
                fprintf(stderr, "Bad order issued! \n");
                break;

        }

    }
}
