#ifndef RESPONSES_H
#define RESPONSES_H

#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include "service_cron.h"

extern Preferences preferences;

void responses_reset (AsyncWebServerRequest *request);
void responses_restart (AsyncWebServerRequest *request);
void responses_NewDevice (AsyncWebServerRequest *request);
void responses_on (AsyncWebServerRequest *request);
void responses_off (AsyncWebServerRequest *request);
void responses_getSchedule (AsyncWebServerRequest *request);
void responses_setSchedule (AsyncWebServerRequest *request);
void responses_wifiStauts (AsyncWebServerRequest *request);
void responses_SSIDlist (AsyncWebServerRequest *request);
void responses_wifi_setting (AsyncWebServerRequest *request);






#endif