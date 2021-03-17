#ifndef SWITCH_TOUCH_H
#define SWITCH_TOUCH_H

#include <Arduino.h>

const int threshold = 20;
bool triggeredT8 = false;
bool triggeredT9 = false;

const long touchDelay = 350; //ms

volatile unsigned long lastT8 = 0;
volatile unsigned long lastT9 = 0;

bool touchDelayComp(unsigned long);

void IRAM_ATTR T8wasActivated();
void IRAM_ATTR T9wasActivated();
void touch_manual();


#endif