#ifndef SERVICE_WIFI_AP_H
#define SERVICE_WIFI_AP_H

#include <WiFi.h>

extern WiFiClass WiFi;

bool wifi_ap_start(String Boardcast_SSID);

#endif