#include "switch_touch.h"

void IRAM_ATTR T8wasActivated() { triggeredT8 = true; }
void IRAM_ATTR T9wasActivated() { triggeredT9 = true; }

bool touchDelayComp(unsigned long lastTouch){
  if (millis() - lastTouch < touchDelay) return false;
  return true;
}

void touch_manual() {
  if (triggeredT8){
    triggeredT8 = false;
    if (touchDelayComp(lastT8)){
      Serial.println("T8");
      //On();
      lastT8 = millis();
    }
  }

  if (triggeredT9){
    triggeredT9 = false;
    if (touchDelayComp(lastT9)){
      Serial.println("T9");
      //Off();
      lastT9 = millis();
    }
  }
}

void touch_auto() {
  if (triggeredT8){
    triggeredT8 = false;
    if (touchDelayComp(lastT8)){
      Serial.println("T8");
      //On();
      lastT8 = millis();
    }
  }

  if (triggeredT9){
    triggeredT9 = false;
    if (touchDelayComp(lastT9)){
      Serial.println("T9");
      //Off();
      lastT9 = millis();
    }
  }
}