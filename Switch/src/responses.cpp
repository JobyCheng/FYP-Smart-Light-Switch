#include "responses.h"

void responses_reset (AsyncWebServerRequest *request){
    Serial.println("\nReset");
    request->send(200);
    Serial.println("Clear all setting");
    preferences.begin("setting");
    preferences.clear();
    preferences.end();
    ESP.restart();
}

void responses_restart (AsyncWebServerRequest *request){
    Serial.println("\nRestart");
    request->send(200);
    ESP.restart();
}

void responses_NewDevice (AsyncWebServerRequest *request){
    Serial.println("\nNewDevice");
    request->send(200);
}

void responses_on (AsyncWebServerRequest *request){
    Serial.println("\nGet:\t\tOn");
    request->send(200);
}

void responses_off (AsyncWebServerRequest *request){
    Serial.println("\nGet:\t\tOff");
    request->send(200);
}

void responses_getSchedule (AsyncWebServerRequest *request){
    Serial.println("\nGet:\t\tGet Schedule");
    request->send(200, "application/json", cron_get_list_json());
}

void responses_setSchedule (AsyncWebServerRequest *request){
    Serial.println("\nPOST:\t\tSet Schedule");
    preferences.begin("setting");
    cron_clear();
    int i = 0;
    while(request->hasParam(String(i).c_str(), true)){
        String data = request->getParam(String(i).c_str(), true)->value();
        cron_add(data);
        preferences.putString(cron_key(i).c_str(), data);
        ++i;
    }
    preferences.end();
    request->send(200);
}

void responses_wifiStauts (AsyncWebServerRequest *request){
    auto status = WiFi.status();
    Serial.println("\nGet:\t\tWiFi Status");
    String message;
    switch (status){
      case WL_CONNECTED:
        message = "Connected";
        break;
      case WL_IDLE_STATUS:
        message = "Connecting...";
        break;
      case WL_CONNECT_FAILED:
        message = "Fail to connect";
        break;
      case WL_CONNECTION_LOST:
        message = "Disconnected";
        break;
      case WL_DISCONNECTED:
        message = "Disconnected";
        break;
      case WL_NO_SHIELD:
        message = "No setting stored";
        break;
      default:
        message = "Others";
        break;
    }
    Serial.println(message);

    request->send(200, "text/plain", message);
}

void responses_SSIDlist (AsyncWebServerRequest *request){
    Serial.println("\nGet:\t\tSSID");
    int n = WiFi.scanComplete();
    // Scanning or do not start yet
    // so start the service
    if(n < 0){
      WiFi.scanDelete();
      WiFi.scanNetworks(true);
      request->send(202);
      Serial.println("Respond:\t202");
      return;
    }

    Serial.println("No. of result:\t"+String(n));
    String json = "[";
    for (int i = 0; i < n; ++i){
      Serial.println("\t"+WiFi.SSID(i));
      if(i) {json += ",";};
      json += "{";
      json += "\"ssid\":\""+WiFi.SSID(i)+"\"";
      json += "}";
    }
    json += "]";
    request->send(200, "application/json", json);
    Serial.println("Respond:\t200");

    WiFi.scanDelete();
}

void responses_wifi_setting (AsyncWebServerRequest *request){
    Serial.println("\nChange wifi setting");
    
    int key_total = 3;
    String keys[key_total] = {"SSID","hidden_SSID","passwd"};
    for(int i = 0; i<key_total; ++i){
      if(!(request->hasParam(keys[i], true))){request->send(202, "text/plain", "Missing data: "+keys[i]); return;}
    }

    String SSID = request->getParam("SSID", true)->value();
    String hidden_SSID = request->getParam("hidden_SSID", true)->value();
    String passwd = request->getParam("passwd", true)->value();

    bool data_not_valid = (passwd.length()<8)||(passwd.length()>63)||(SSID=="");
    if(data_not_valid){request->send(202, "text/plain", "Invaild Data");return;}

    request->send(200, "text/plain", "New setting applied, wait until restart");
    
    if (SSID == "hidden"){ SSID=hidden_SSID; }
    
    preferences.begin("setting");
    preferences.putString("SSID", SSID);
    preferences.putString("PASSWD", passwd);
    preferences.end();
    
    ESP.restart();
}