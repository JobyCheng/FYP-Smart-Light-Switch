#ifndef SWITCH_GY25Z_H
#define SWITCH_GY25Z_H

#include <Arduino.h>

extern float roll;
extern unsigned char Re_buf[11],counter;
extern unsigned char sign;

void gyro_init();
void gyro_resetRollPitch();
void gyro_resetYaw();
void gyro_autoOutputMode();
void gyro_queryOutputMode();
float gyro_getRoll();

#endif