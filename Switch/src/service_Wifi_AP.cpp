#include "service_Wifi_AP.h"

bool wifi_ap_start(String Boardcast_SSID){
    if (!WiFi.softAP(Boardcast_SSID.c_str())){
        Serial.println("WiFi_AP:\tFail");
        return false;
    }
    Serial.println("WiFi_AP:\tOK");
    Serial.println("Name:\t\t"+Boardcast_SSID);
    Serial.print("AP address:\t");
    Serial.println(WiFi.softAPIP());
    return true;
}