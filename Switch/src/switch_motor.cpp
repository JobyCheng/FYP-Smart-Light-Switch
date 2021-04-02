#include "switch_motor.h"


const int AIN1 = 25; 
const int AIN2 = 26; 
const int EN = 27;
float T8Roll;
float T9Roll;

//clk = t9
//anticlk = t8

void on(){
    preferences.begin("switchSetting");
    if(preferences.getBool("T8IsOn", false)){
        turnClkwise();
    }
    else{
        turnAntiClkwise();
    }
}

void off(){
    preferences.begin("switchSetting");
    if(preferences.getBool("T8IsOn", false)){
        turnAntiClkwise();
    }
    else{
        turnClkwise();
    }
}

void recalibrate(){
    Serial.println("test");
    bool cal = true;
    bool calT8 = false;
    bool calT9 = false;
    digitalWrite(EN, HIGH);
    while (cal){
        if(triggeredT8 && !triggeredT9){
            //Serial.println("T8");
            digitalWrite(AIN1, LOW);
            digitalWrite(AIN2, HIGH);
            if(touchRead(T8)>threshold){
                digitalWrite(AIN1, LOW);
                digitalWrite(AIN2, LOW);
                triggeredT8 = false;
                calT8 = true;
                T8Roll = gyro_getRoll();
                delay(1);
            }
            //delay(200);
        }
        if(triggeredT9 && !triggeredT8){
            //Serial.println("T9");
            digitalWrite(AIN1, HIGH);
            digitalWrite(AIN2, LOW);
            if(touchRead(T9)>threshold){
                digitalWrite(AIN1, LOW);
                digitalWrite(AIN2, LOW);
                triggeredT9 = false;
                calT9 = true;
                T9Roll = gyro_getRoll();
                delay(1);
            } 
            //delay(200);
        }
        if(calT8 && calT9){
            
            delay(1);
            digitalWrite(EN, LOW);
            preferences.begin("switchSetting");
            preferences.putFloat("T8Roll",T8Roll);
            preferences.putFloat("T9Roll",T9Roll);
            if(T8Roll > T9Roll){
                preferences.putBool("T8IsOn",true);
            }else{
                preferences.putBool("T8IsOn",false);
            }
            delay(1);
            Serial.print("T8Roll: "); 
            Serial.println(preferences.getFloat("T8Roll",-1));
            Serial.print("T9Roll: "); 
            Serial.println(preferences.getFloat("T9Roll",-1));
            Serial.println(preferences.getBool("T8IsOn", false));
            preferences.end();
            
            cal = false;
            Serial.println("end cal");
            //delay(1000);
        }
        //delay(200);
    }
    return;
}

void turnClkwise(){
    preferences.begin("switchSetting");
    digitalWrite(EN, HIGH);
    float current_roll = gyro_getRoll();
    float target_roll = preferences.getFloat("T8Roll",-1);
    while(current_roll< target_roll && abs(current_roll - target_roll)>2){
        digitalWrite(AIN1, LOW);
        digitalWrite(AIN2, HIGH);
    }
    digitalWrite(EN, LOW);
    preferences.end();
}

void turnAntiClkwise(){
    preferences.begin("switchSetting");
    digitalWrite(EN, HIGH);
    float current_roll = gyro_getRoll();
    float target_roll = preferences.getFloat("T9Roll",-1);
    while(current_roll > target_roll && abs(current_roll - target_roll)>2){
        digitalWrite(AIN1, HIGH);
        digitalWrite(AIN2, LOW);
    }
    digitalWrite(EN, LOW);
    preferences.end();
}

void touch_manual() {
  if (triggeredT8){
    triggeredT8 = false;
    if (touchDelayComp(lastT8)){
      Serial.println("turn anticlkwise");
      turnAntiClkwise();
      lastT8 = millis();
    }
  }

  if (triggeredT9){
    triggeredT9 = false;
    if (touchDelayComp(lastT9)){
      Serial.println("turn clkwise");
      turnClkwise();
      lastT9 = millis();
    }
  }
}