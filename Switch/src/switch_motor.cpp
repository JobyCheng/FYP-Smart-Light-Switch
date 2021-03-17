#include "switch_motor.h"

const int AIN1 = 25; 
const int AIN2 = 26; 
const int EN = 27;

void on(){
    digitalWrite(EN, HIGH);
    //ad(T))
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    delay(1000);
    digitalWrite(EN, LOW);
}

void off(){
    digitalWrite(EN, HIGH);
    //ad(T))
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    delay(1000);
    digitalWrite(EN, LOW);
}

void recalibrate(){
    digitalWrite(EN, HIGH);
    while(touchedT8){
        digitalWrite(AIN1, HIGH);
        digitalWrite(AIN2, LOW);
    }
    while(touchedT9){
        
    }
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