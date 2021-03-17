#include "switch_gy25z.h"

void gyro_init(){
    Serial2.begin(115200);
    
    Serial2.write(0XA5);
    Serial2.write(0X55); 
    Serial2.write(0X10);
    Serial2.write(0X0A);
    delay(3000);

    Serial2.write(0XA5); 
    Serial2.write(0X56);
    Serial2.write(0X01); 
    Serial2.write(0XFC);
    delay(100);

    Serial2.write(0XA5);
    Serial2.write(0X5A); 
    Serial2.write(0X01);
    Serial2.write(0X00);
    delay(100);
    
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

void gyro_getRoll(){
    while (Serial2.available()){
    Re_buf[counter]=(unsigned char)Serial2.read();
    //Serial.println(Re_buf[counter]);
    if(counter==0&&Re_buf[0]!=0x5A){
        Serial.print("gyro data error");
        break;
    } 
    counter++; 
    if(counter==11){    
       counter=0;                 //重新赋值，准备下一帧数据的接收 
       sign=1;
       break;
    }
  }
  if(sign){  
     sign=0;
     if(Re_buf[0]==0x5A&&Re_buf[1]==0x5A){
        roll=(Re_buf[4]<<8|Re_buf[5])/100;
        Serial.print("roll: ");
        Serial.println(roll, DEC);
        delay(100);
     }
  }
}

