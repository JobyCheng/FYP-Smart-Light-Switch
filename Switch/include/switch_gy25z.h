#ifndef SWITCH_GY25Z_H
#define SWITCH_GY25Z_H

#include <Arduino.h>

extern float roll;
extern unsigned char Re_buf[11],counter;
extern unsigned char sign;

void gy25z_init();
void gy25z_resetRollPitch();
void gy25z_resetYaw();
void gy25z_autoOutputMode();
void gy25z_queryOutputMode();
float gy25z_getRoll();

#endif