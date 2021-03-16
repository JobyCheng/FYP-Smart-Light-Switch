#ifndef GY25Z_H
#define GY25Z_H

#include <Arduino.h>

void gyro_init();
void gyro_resetRollPitch();
void gyro_resetYaw();
void gyro_autoOutputMode();
void gyro_queryOutputMode();

#endif