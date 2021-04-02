#ifndef SWITCH_MOTOR_H
#define SWITCH_MOTOR_H

#include <Arduino.h>
#include "switch_gy25z.h"
#include "switch_touch.h"
#include <Preferences.h>
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

extern Preferences preferences;

//motor control pins
extern const int AIN1; 
extern const int AIN2; 
extern const int EN;

extern float T8roll;
extern float T9roll;
//extern unsigned char cal;


void on();
void off();
void recalibrate();
void manual_switch();
void turnClkwise();
void turnAntiClkwise();

#endif