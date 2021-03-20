#include "switch_motor.h"

const int AIN1 = 25; 
const int AIN2 = 26; 
const int EN = 27;
float T8Roll;
float T9Roll;

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
    unsigned long lastTouch = millis();
    digitalWrite(EN, HIGH);
    while(triggeredT8 && !triggeredT9){
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, HIGH);
        T8Roll = gyro_getRoll();
        lastTouch = millis();
    }
    while(triggeredT9 && !triggeredT8){
        digitalWrite(AIN1, HIGH);
        digitalWrite(AIN2, LOW);
        T9Roll = gyro_getRoll();
        lastTouch = millis();
    }
    if((millis() - lastTouch) < long(350)){

    }
}

void turnClkwise(){
    float roll = gyro_getRoll();
    
}

void turnAntiClkwise(){
    
}

void touch_manual() {
  if (triggeredT8){
    triggeredT8 = false;
    if (touchDelayComp(lastT8)){
      Serial.println("turn clkwise");
      //On();
      lastT8 = millis();
    }
  }

  if (triggeredT9){
    triggeredT9 = false;
    if (touchDelayComp(lastT9)){
      Serial.println("turn anticlkwise");
      //Off();
      lastT9 = millis();
    }
  }
}