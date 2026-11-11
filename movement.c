/*
 * movement.c
 *
 *  Created on: Sep 2, 2026
 *      Author: gapelaez
 */

#include "movement.h"
#include "Timer.h"
#include "math.h"
#include <math.h>
#include "uart.h"

#define TURN_K 2.0
#define TURN_D 0
#define MOVE_K 1.5
#define MOVE_D 0.0
#define TURN_CORRECTOR 20
#define OFFSET 3.0

char str[50];

void turn(ActionStack* stack, int angle, int time) {
    Action action = {};
    action.dist = angle;
    action.isTurn = 1;
    action.time = time;
    action.doCollision = 0;
    push(stack, action);
}
void forward_move(ActionStack* stack, float dist, int time, int doCollision)
{
    Action action = {};
    action.dist = dist - OFFSET;
    action.isTurn = 0;
    action.time = time;
    action.doCollision = doCollision;

    push(stack, action);
}

// READ FROM BOTTOM TO TOP BECAUSE IT IS LIFO
int doCollision(oi_t* sensor_data, ActionStack* stack, float remainingDist) {
    //backup when hit and tell user
    if(sensor_data->bumpLeft && sensor_data->bumpRight) {
        forward_move(stack, -150, 1000, 0);
        run(stack);
        sprintf(str, "bump on middle! do manual control!\n\r");
        uart_send_str(str);
        return 1;

    } else if(sensor_data->bumpRight) {
        forward_move(stack, -150, 1000, 0);
        run(stack);
        sprintf(str, "bump on right! do manual control!\n\r");
        uart_send_str(str);
        return 1;
    } else if(sensor_data->bumpLeft) {
        forward_move(stack, -150, 1000, 0);
        run(stack);
        sprintf(str, "bump on left! do manual control!\n\r");
        uart_send_str(str);
        return 1;
    }
    return 0;
}
// execute action with pidloop
void doAction(ActionStack* stack, Action* action, oi_t *sensor_data) {
    if(action->isTurn) {
        double angleSum = 0;
        int startTime = timer_getMillis();
        while ((timer_getMillis() - startTime) < action->time)
        {
            // calulate error
            oi_update(sensor_data);
            angleSum += sensor_data->angle;
            double error = action->dist - angleSum;
            int power = -TURN_K * error; //intentional conversion to int
            // clamp values
            if (power > 250)
            {
                power = 250;
            }
            if (power < -250)
            {
                power = -250;
            }
            oi_setWheels(-power, power);
        }
        oi_setWheels(0, 0);
    } else {
        float distSum = 0;
        int startTime = timer_getMillis();
        int past_time = startTime;
        float past_error = 0;
        while ((timer_getMillis() - startTime) < action->time)
        {
            oi_update(sensor_data);
            if(action->doCollision) {
                int collided = doCollision(sensor_data, stack, action->dist - distSum);
                if(collided) {
                    oi_setWheels(0, 0);
                    return;
                }
            }
            distSum += sensor_data->distance;
            float error = action->dist - distSum;

            float derror = 0;
            int curr_time = timer_getMillis();
            int dt = curr_time - past_time;
            if(dt > 0) {
                derror = 1000.0 * ((error - past_error) / (float)dt);
                past_time = curr_time;
                past_error = error;
            }
            int power = MOVE_K * error + MOVE_D * derror; //intentional conversion to int
            if (power > 250)
            {
                power = 250;
            }
            if (power < -250)
            {
                power = -250;
            }
            oi_setWheels(power, power); //flipped for left turn
        }
        oi_setWheels(0, 0);
    }
}
ActionStack createStack() {
    ActionStack stack = {};
    stack.top = -1;
    stack.size = sizeof(stack.actions) / sizeof(Action);
    return stack;
}
int isEmpty(ActionStack* stack) {
    if(stack->top == -1) return 1;
    return 0;
}
int isFull(ActionStack* stack) {
    if (stack->top > stack->size) return 1;
    return 0;
}
void push(ActionStack* stack, Action action) {
    if(stack->top >= stack->size - 1) {
        lcd_printf("Stack is full!");
        return;
    }
    stack->top++;
    stack->actions[stack->top] = action;
}
Action pop(ActionStack* stack) {
    Action action = stack->actions[stack->top];
    stack->top--;
    return action;
}

void run(ActionStack* stack) {
    while(!isEmpty(stack)) {
        Action action = pop(stack);
        doAction(stack, &action, stack->sensor_data);
    }
}
