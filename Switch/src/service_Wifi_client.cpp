#include "service_Wifi_client.h"

bool wifi_client_start_with_setting(){
    preferences.begin("setting");
    String SSID = preferences.isKey("SSID")?preferences.getString("SSID"):String();
    String PASSWD = preferences.isKey("PASSWD")?preferences.getString("PASSWD"):String();
    preferences.end();

    if(SSID == ""){Serial.println("Wifi Setting not found");return false;};
    WiFi.begin(SSID.c_str(), PASSWD.c_str());

    if(WiFi.waitForConnectResult()!=WL_CONNECTED){Serial.println("Unable to connect to " + SSID);return false;}

    Serial.println("WiFi:\t\tOK");
    Serial.println("IP address:\t" + String(WiFi.localIP()));

    // Power Saving Option, lower wifi client power usage
    esp_wifi_set_ps(WIFI_PS_MIN_MODEM);
    return true;
}
