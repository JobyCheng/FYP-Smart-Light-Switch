#include "switch_motor.h"


const int AIN1 = 25; 
const int AIN2 = 26; 
const int EN = 27;
volatile float T8Roll;
volatile float T9Roll;
bool calManual = false;


//clk = t8
//anticlk = t9

void on(){
    //Serial.print("Switch On");
    
    preferences.begin("switchSetting"); 
    if(preferences.getBool("T8IsOn", false)){
        turnClkwise();
    }else{
        turnAntiClkwise();
    }
    preferences.end();
}

void off(){
    //Serial.print("off");

    preferences.begin("switchSetting");
    if(preferences.getBool("T8IsOn", false)){
        turnAntiClkwise();
    }else{
        turnClkwise();
    }
    preferences.end();
}

void calibrate(){
    //Serial.println("test");
    delay(700);
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
                T8Roll = gy25z_getRoll();
                delay(50);
            }
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
                T9Roll = gy25z_getRoll();
                delay(50);
            }
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
            Serial.print("T8IsOn:" );
            Serial.println(preferences.getBool("T8IsOn", false)?String("True"):String("False"));
            preferences.putBool("calibrated",true);
            preferences.end();
            
            cal = false;
            calManual = true;
            Serial.println("end cal");
        }
    }
    return;
}

void turnClkwise(){
    preferences.begin("switchSetting");
    if(preferences.getBool("calibrated",false)){
        preferences.putBool("switchIsOn", (preferences.getBool("T8IsOn", false)?true:false));
        digitalWrite(EN, HIGH);
        float current_roll = gy25z_getRoll();
        float target_roll = preferences.getFloat("T8Roll",-1);
        digitalWrite(AIN1, LOW);
            digitalWrite(AIN2, HIGH);
        if(abs(current_roll - target_roll)<=2){
            digitalWrite(EN, LOW);
        }
    }else{
        Serial.println("Plz calibrate first");
    }
    preferences.end();
}

void turnAntiClkwise(){
    preferences.begin("switchSetting");
    if(preferences.getBool("calibrated",false)){
        preferences.putBool("switchIsOn", (preferences.getBool("T8IsOn", false)?false:true));
        digitalWrite(EN, HIGH);
        float current_roll = gy25z_getRoll();
        float target_roll = preferences.getFloat("T9Roll",-1);
        digitalWrite(AIN1, HIGH);
        digitalWrite(AIN2, LOW);
        if(abs(current_roll - target_roll)<=2){
            digitalWrite(EN, LOW);
        }
    }else{
        Serial.println("Plz calibrate first");
    }
    preferences.end();
}

void touch_manual() {
  if (triggeredT8 && calManual){
    triggeredT8 = false;
    if (touchIntervalComp(lastT8)){
      //Serial.println("turn anticlkwise");
      turnClkwise();
      lastT8 = millis();
    }
  }

  if (triggeredT9 && calManual){
    triggeredT9 = false;
    if (touchIntervalComp(lastT9)){
      //Serial.println("turn clkwise");
      turnAntiClkwise();
      lastT9 = millis();
    }
  }
}