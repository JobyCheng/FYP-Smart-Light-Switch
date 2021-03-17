#ifndef SWITCH_GY25Z_H
#define SWITCH_GY25Z_H

#include <Arduino.h>

extern int16_t roll;
extern uint8_t Re_buf[11],counter;
extern uint8_t sign;

void gyro_init();
void gyro_resetRollPitch();
void gyro_resetYaw();
void gyro_autoOutputMode();
void gyro_queryOutputMode();

#endif