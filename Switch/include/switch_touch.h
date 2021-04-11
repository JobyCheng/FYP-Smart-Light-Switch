#ifndef SWITCH_TOUCH_H
#define SWITCH_TOUCH_H

#include <Arduino.h>

extern const int threshold;
extern bool triggeredT8;
extern bool triggeredT9;

extern const long minInterval; //ms

extern volatile unsigned long lastT8;
extern volatile unsigned long lastT9;

extern bool touchIntervalComp(unsigned long);

void IRAM_ATTR T8wasActivated();
void IRAM_ATTR T9wasActivated();
void touch_manual();


#endif