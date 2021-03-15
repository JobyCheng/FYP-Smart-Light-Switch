#ifndef SERVICE_WIFI_CLIENT_H
#define SERVICE_WIFI_CLIENT_H

#include <WiFi.h>
#include <Preferences.h>
#include "esp_wifi.h"

extern WiFiClass WiFi;
extern Preferences preferences;

bool wifi_client_start_with_setting();

#endif