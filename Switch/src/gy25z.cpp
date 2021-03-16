#include "gy25z.h"

void gyro_init(){
    Serial2.begin(115200);
    
    Serial2.write(0XA5);
    Serial2.write(0X55); 
    Serial2.write(0X10);
    Serial2.write(0X0A);
    delay(3000);
    
    Serial.println("gyro start");
}

void gyro_resetRollPitch(){
    Serial2.write(0XA5); 
    Serial2.write(0X57);
    Serial2.write(0X01); 
    Serial2.write(0XFD);
    delay(3000);
}

void gyro_resetYaw(){
    Serial2.write(0XA5); 
    Serial2.write(0X57);
    Serial2.write(0X02); 
    Serial2.write(0XFE);
    delay(3000);
}

void gyro_autoOutputMode(){
    Serial2.write(0XA5); 
    Serial2.write(0X56);
    Serial2.write(0X02); 
    Serial2.write(0XFD);
    delay(1000);
}

void gyro_queryOutputMode(){
    Serial2.write(0XA5); 
    Serial2.write(0X56);
    Serial2.write(0X01); 
    Serial2.write(0XFC);
    delay(1000);
}