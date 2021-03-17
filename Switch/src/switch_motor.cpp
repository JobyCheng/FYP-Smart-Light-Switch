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
    
}