/*
 * movement.h
 *
 *  Created on: Sep 2, 2026
 *      Author: gapelaez
 */

#ifndef MOVEMENT_H_
#define MOVEMENT_H_
#include "open_interface.h"

typedef struct Action {
    int isTurn;
    float dist;
    int time;
    int doCollision;
} Action;

typedef struct ActionStack {
    Action actions[10];
    int top;
    int size;
    oi_t* sensor_data;
} ActionStack;

ActionStack createStack();
int isFull(ActionStack* stack);
int isEmpty(ActionStack* stack);
void push(ActionStack* stack, Action action);
Action pop(ActionStack* stack);

void doAction(ActionStack* stack, Action* action, oi_t *sensor_data);
void forward_move(ActionStack* stack, float dist, int time, int doCollision);
void turn(ActionStack* stack, int angle, int time);
void auto_move(ActionStack* stack, float distance,int target_ang);

#endif /* MOVEMENT_H_ */
