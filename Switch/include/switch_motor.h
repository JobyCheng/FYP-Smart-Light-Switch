#ifndef SWITCH_MOTOR_H
#define SWITCH_MOTOR_H

#include <Arduino.h>
#include "switch_gy25z.h"
#include "switch_touch.h"

//motor control pins
extern const int AIN1; 
extern const int AIN2; 
extern const int EN; 


void on();
void off();
void recalibrate();
void manual_switch();

#endif