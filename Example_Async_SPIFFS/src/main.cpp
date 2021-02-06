#include <Arduino.h>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

#include "setting.h"

AsyncWebServer server(80);
 
void setup(){
  Serial.begin(115200);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println(WiFi.localIP());

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

// the index page 
  server.serveStatic("/", SPIFFS, "/www").setDefaultFile("index.html");

// get request: scan
  server.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("recive GET request");
    String json = "[";
    int n = WiFi.scanComplete();
    if(n == -2){
      WiFi.scanNetworks(true);
    } else if(n){
      for (int i = 0; i < n; ++i){
        if(i) json += ",";
        json += "{";
        json += "\"rssi\":"+String(WiFi.RSSI(i));
        json += ",\"ssid\":\""+WiFi.SSID(i)+"\"";
        json += ",\"bssid\":\""+WiFi.BSSIDstr(i)+"\"";
        json += ",\"channel\":"+String(WiFi.channel(i));
        json += ",\"secure\":"+String(WiFi.encryptionType(i));
        json += "}";
      }
      WiFi.scanDelete();
      if(WiFi.scanComplete() == -2){
        WiFi.scanNetworks(true);
      }
    }
    json += "]";
    request->send(200, "application/json", json);
    json = String();
  });

  // post request: 
  server.on("/postText",HTTP_POST,[](AsyncWebServerRequest *request){
    Serial.println("recive POST request");
    request->send(200, "text/plain", "Server recived");

    if(request->hasParam("fname", true)){
      String temp = request->getParam("fname", true)->value();
      Serial.println(temp);
    }
    if(request->hasParam("lname", true)){
      String temp = request->getParam("lname", true)->value();
      Serial.println(temp);
    }
    });


  server.begin();
}
 
void loop(){}