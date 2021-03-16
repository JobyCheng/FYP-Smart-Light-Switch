#include "switch_basic.h"

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