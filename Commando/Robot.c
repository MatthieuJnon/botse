#include <stdlib.h>
#include <stdio.h>
#include "prose.h"
#include "Robot.h"

#define LEFT_MOTOR MD
#define RIGHT_MOTOR MA
#define LIGHT_SENSOR S1
#define FRONT_BUMPER S3
#define FLOOR_SENSOR S2 //What is this?



static Robot * robot;


//Okay finished
extern void Robot_new() {

    robot = (Robot *) malloc(sizeof(Robot)); //Todo : verify with if or assert!

    /* Initialisation des moteurs. */
    robot->left_motor = Motor_open(LEFT_MOTOR);
    if (robot->left_motor == NULL) PProseError("Problème d'ouverture du moteur gauche (port MD)");

    robot->right_motor = Motor_open(RIGHT_MOTOR);
    if (robot->right_motor == NULL) PProseError("Problème d'ouverture du moteur droit (port MA)");

    robot->contact_sensor = ContactSensor_open(FRONT_BUMPER);
    if (robot->contact_sensor == NULL) PProseError("Problème d'ouverture du contact_sensor (port S3)");

    robot->light_sensor = LightSensor_open(LIGHT_SENSOR);
    if (robot->light_sensor == NULL) PProseError("Problème d'ouverture du light_sensor (port S1)");


}

//Okay finished
extern void Robot_free() {
    /* Fermeture des accès aux moteurs. */
    Motor_close(robot->left_motor);
    Motor_close(robot->right_motor);

    LightSensor_close(robot->light_sensor);
    ContactSensor_close(robot->contact_sensor);

    free(robot);
}

//Is this the point of this function?
extern void Robot_stop() {
	/* Arrêt des moteurs. Utile ? */
    //Robot_setVelocity(0, 0);

    ProSE_Intox_close();
}

//Is this the point of this function?
extern void Robot_start(){
    if (ProSE_Intox_init("127.0.0.1", 12345) == -1) {
        PProseError("Problème d'initialisation du simulateur Intox");
    }
}

//Okay finished
extern void Robot_setVelocity(int mr,int ml) {
    /* Appliquer les consignes aux moteurs. */
    if (Motor_setCmd(robot->left_motor, ml) == -1) {
        PProseError("Problème de commande du moteur gauche");
    }
    if (Motor_setCmd(robot->right_motor, mr) == -1) {
        PProseError("Problème de commande du moteur droit");
    }
}

//Okay finished
extern int Robot_getRobotSpeed(){
    //Ici calcul necessaire je suppose ! Avec les deux roues!
    Cmd v_right = Motor_getCmd(robot->right_motor);
    if (v_right == NULL) \
        PProseError("Problème lors de la récupération de la vitesse moteur droit!");

    Cmd v_left = Motor_getCmd(robot->left_motor);
    if (v_left == NULL) \
        PProseError("Problème lors de la récupération de la vitesse moteur gauche!");

    return ((v_right + v_left) /2); //En attente du calcul approprié!

}

//Okay Finished
extern SensorState Robot_getSensorState(){
    SensorState out;

    ContactStatus collision_temp;


    collision_temp = ContactSensor_getStatus(robot->contact_sensor);
    if (collision_temp == ERROR){
        PProseError("Problème lors de la récupération de données sur le capteur de collision!");
    }
    else{
        if (collision_temp == PRESSED){
            out.collision = BUMPED;
        }
        else if (collision_temp == RELEASED){
            out.collision = NO_BUMP;
        }
    }

    out.luminosity = LightSensor_getStatus(robot->light_sensor);
    if (out.luminosity == -1){
        PProseError("Problème lors de la récupération de données sur le capteur de luminosité!");
    }

    return out;
}

