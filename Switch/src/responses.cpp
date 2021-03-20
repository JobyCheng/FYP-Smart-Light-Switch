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
    if((request->hasParam("id"))&&request->hasParam("label")){
      request->send(200);
      client_list.push_back({request->getParam("id")->value(),request->getParam("label")->value()});
    }
}

void responses_on (AsyncWebServerRequest *request){
    Serial.println("\nGet:\t\tOn");
    on();
    request->send(200);
}

void responses_off (AsyncWebServerRequest *request){
    Serial.println("\nGet:\t\tOff");
    off();
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

    if(n < 0){     // Scanning or do not start yet
      request->send(202);
      Serial.println("Respond:\t202");
      WiFi.scanDelete();
      WiFi.scanNetworks(true);
      return;
    }

    Serial.println("No. of result:\t"+String(n));
    String json = "[";
    for (int i = 0; i < n; ++i){
      if(i) {json += ",";};
      json += "{\"ssid\":\""+WiFi.SSID(i)+"\"}";
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

void responses_getClient (AsyncWebServerRequest *request){
  Serial.println("\nGet:\t\tClient List");
  String json = "[";
  for (int i = 0; i < client_list.size(); ++i){
    if(i) {json += ",";};
    json += "{";
    json += "\"id\":\""+client_list[i].id+"\",";
    json += "\"label\":\""+client_list[i].label+"\"";
    json += "}";
  }
  json += "]";

  request->send(200, "application/json", json);
}

void responses_info (AsyncWebServerRequest *request){
  Serial.println("\nGet:\t\tInfo");
  String json = "[{";
  json += "\"id\":\""+DEVICE_ID+"\",";
  json += "\"label\":\""+LABEL+"\",";
  json += "\"status\":"+String("false"); // NEED TO BE CHANGE!!!!!!!!!!!!!!!!!!!!!!
  json += "}]";

  AsyncWebServerResponse *response = request->beginResponse(200, "application/json", json);
  response->addHeader("Access-Control-Allow-Origin","http://"+PRODUCT_NAME+".local");
  request->send(response);
}

void responses_setLabel (AsyncWebServerRequest *request){
  Serial.println("\nGet:\t\tSet label");
  request->send(200);
  if(request->hasParam("label")){
    LABEL = request->getParam("label")->value();
    Serial.println("New label:\t"+LABEL);
    preferences.begin("setting");
    preferences.putString("LABEL", LABEL);
    preferences.end();
  }
}

void responses_calibration (AsyncWebServerRequest *request){
    Serial.println("\nGet:\t\tCalibration");
    request->send(200);
}
