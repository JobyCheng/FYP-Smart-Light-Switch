#ifndef RESPONSES_H
#define RESPONSES_H

#include "vector"
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include "service_cron.h"
#include "switch_motor.h"

struct client_entry{
    String id;
    String label;
};

extern Preferences preferences;
extern std::vector<client_entry> client_list;
extern String DEVICE_ID;
extern String LABEL;
extern String MAC_ADDR;

void responses_reset (AsyncWebServerRequest *request);
void responses_restart (AsyncWebServerRequest *request);
void responses_NewDevice (AsyncWebServerRequest *request);
void responses_on (AsyncWebServerRequest *request);
void responses_off (AsyncWebServerRequest *request);
void responses_recalibrate (AsyncWebServerRequest *request);
void responses_getSchedule (AsyncWebServerRequest *request);
void responses_setSchedule (AsyncWebServerRequest *request);
void responses_wifiStauts (AsyncWebServerRequest *request);
void responses_SSIDlist (AsyncWebServerRequest *request);
void responses_wifi_setting (AsyncWebServerRequest *request);
void responses_getClient (AsyncWebServerRequest *request);
void responses_info (AsyncWebServerRequest *request);
void responses_setLabel (AsyncWebServerRequest *request);

#endif