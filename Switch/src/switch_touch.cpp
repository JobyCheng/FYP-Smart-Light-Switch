#include "switch_touch.h"

bool triggeredT8 = false;
bool triggeredT9 = false;
void IRAM_ATTR T8wasActivated() { triggeredT8 = true; }
void IRAM_ATTR T9wasActivated() { triggeredT9 = true; }

volatile unsigned long lastT8 = 0;
volatile unsigned long lastT9 = 0;

const int threshold = 20;

const long touchDelay = 350;

bool touchDelayComp(unsigned long lastTouch){
    if ((millis() - lastTouch) < touchDelay) return false;
    return true;
}

