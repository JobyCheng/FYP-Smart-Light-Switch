#ifndef SWITCH_BASIC_H
#define SWITCH_BASIC_H

#include <Arduino.h>

const int AIN1 = 25; 
const int AIN2 = 26; 
const int EN = 27; 

void on();
void off();
void recalibrate();
void manual_switch();

#endif