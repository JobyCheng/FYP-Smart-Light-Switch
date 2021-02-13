#include <Arduino.h>

#include <Preferences.h>
// Library for Network
#include <WiFi.h>
#include <ESPmDNS.h>
#include <DNSServer.h>
// Library for Webserver
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

//WIP
//#include <ESP32Time.h>

const uint16_t DNS_PORT = 53;
AsyncWebServer web_server(80);
DNSServer dns_server;
Preferences preferences;

void setDefault(){
  Serial.println("Clear all setting");
  preferences.begin("setting");
  preferences.clear();
  preferences.end();
}

/*
███████╗███████╗████████╗██╗   ██╗██████╗
██╔════╝██╔════╝╚══██╔══╝██║   ██║██╔══██╗
███████╗█████╗     ██║   ██║   ██║██████╔╝
╚════██║██╔══╝     ██║   ██║   ██║██╔═══╝
███████║███████╗   ██║   ╚██████╔╝██║
╚══════╝╚══════╝   ╚═╝    ╚═════╝ ╚═╝

*/
void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  preferences.begin("setting");
  /*
  ███╗   ██╗███████╗████████╗██╗    ██╗ ██████╗ ██████╗ ██╗  ██╗
  ████╗  ██║██╔════╝╚══██╔══╝██║    ██║██╔═══██╗██╔══██╗██║ ██╔╝
  ██╔██╗ ██║█████╗     ██║   ██║ █╗ ██║██║   ██║██████╔╝█████╔╝
  ██║╚██╗██║██╔══╝     ██║   ██║███╗██║██║   ██║██╔══██╗██╔═██╗
  ██║ ╚████║███████╗   ██║   ╚███╔███╔╝╚██████╔╝██║  ██║██║  ██╗
  ╚═╝  ╚═══╝╚══════╝   ╚═╝    ╚══╝╚══╝  ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝

  */
  // Access Point
  String DEVICE_NAME = "esp32";
  String DEVICE_PASSWD = preferences.getString("DEVICE_PASSWD", "12345678");

  IPAddress AP_IP = IPAddress();
  if (WiFi.softAP(DEVICE_NAME.c_str(),DEVICE_PASSWD.c_str())){
    Serial.println("WiFi_AP:\tOK");
    AP_IP = WiFi.softAPIP();
    Serial.println("Name:\t\t"+DEVICE_NAME);
    Serial.println("Password:\t"+DEVICE_PASSWD);
    Serial.print("AP address:\t");
    Serial.println(AP_IP);
  }else{
    Serial.println("WiFi_AP:\tFail");
  }

  // DNS
  if(dns_server.start(DNS_PORT, "*", AP_IP)){
     Serial.println("DNS:\t\tOK");
  }else{
     Serial.println("DNS:\t\tFail");
  }

  // mDNS
  if(MDNS.begin(DEVICE_NAME.c_str())) {
    Serial.println("mDNS:\t\tOK");
    MDNS.addService("http", "tcp", 80);
    Serial.println("address:\thttp://"+DEVICE_NAME+".local");
  }else{
    Serial.println("mDNS:\t\tFail");
  }

  // Wifi client
  String SSID = preferences.getString("SSID", "");
  String PASSWD = preferences.getString("PASSWD", "");
  if(!(SSID == "")){
    Serial.println("Wifi Conf:\tTrue");
    WiFi.begin(SSID.c_str(), PASSWD.c_str());
    if(WiFi.waitForConnectResult()==WL_CONNECTED){
      Serial.println("WiFi:\t\tOK");
      Serial.print("WiFi IP:\t");
      Serial.println(WiFi.localIP());
    }else{
      Serial.println("WiFi:\t\tFail");
    }
  }

  preferences.end();
  
  /*
  ██╗    ██╗███████╗██████╗ ███████╗███████╗██████╗ ██╗   ██╗███████╗██████╗
  ██║    ██║██╔════╝██╔══██╗██╔════╝██╔════╝██╔══██╗██║   ██║██╔════╝██╔══██╗
  ██║ █╗ ██║█████╗  ██████╔╝███████╗█████╗  ██████╔╝██║   ██║█████╗  ██████╔╝
  ██║███╗██║██╔══╝  ██╔══██╗╚════██║██╔══╝  ██╔══██╗╚██╗ ██╔╝██╔══╝  ██╔══██╗
  ╚███╔███╔╝███████╗██████╔╝███████║███████╗██║  ██║ ╚████╔╝ ███████╗██║  ██║
   ╚══╝╚══╝ ╚══════╝╚═════╝ ╚══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝  ╚══════╝╚═╝  ╚═╝

  */
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  web_server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  /*
  ██████╗ ███████╗ ██████╗ ██╗   ██╗███████╗███████╗████████╗
  ██╔══██╗██╔════╝██╔═══██╗██║   ██║██╔════╝██╔════╝╚══██╔══╝
  ██████╔╝█████╗  ██║   ██║██║   ██║█████╗  ███████╗   ██║
  ██╔══██╗██╔══╝  ██║▄▄ ██║██║   ██║██╔══╝  ╚════██║   ██║
  ██║  ██║███████╗╚██████╔╝╚██████╔╝███████╗███████║   ██║
  ╚═╝  ╚═╝╚══════╝ ╚══▀▀═╝  ╚═════╝ ╚══════╝╚══════╝   ╚═╝

  */
  web_server.on("/reset",HTTP_GET,[](AsyncWebServerRequest *request){
    Serial.println("");
    Serial.println("reset");
    request->send(200, "text/plain", "Server recived");
    setDefault();
   });

  web_server.on("/wifiStauts",HTTP_GET,[](AsyncWebServerRequest *request){
    auto status = WiFi.status();
    Serial.println("");
    Serial.println("wifiStauts");
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
      Serial.println(status);

    request->send(200, "text/plain", message);
   });

  web_server.on("/SSIDlist", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("");
    Serial.println("SSID");
    int n = WiFi.scanNetworks(); //Not in Async mode
    String json = "[";
    for (int i = 0; i < n; ++i){
      if(i) {json += ",";};
      json += "{";
      json += "\"ssid\":\""+WiFi.SSID(i)+"\"";
      json += "}";
    }
    WiFi.scanDelete();
    json += "]";
    request->send(200, "application/json", json);
  });

  web_server.on("/AP_passwd",HTTP_POST,[](AsyncWebServerRequest *request){
    Serial.println("");
    Serial.println("Set Access Point password");
    if(request->hasParam("passwd", true)){
      String passwd = request->getParam("passwd", true)->value();
      Serial.println(passwd);
        preferences.begin("setting");
        preferences.putString("DEVICE_PASSWD",passwd);
        preferences.end();
    }
    request->send(200, "text/plain", "Restart for the changes to take effect");
   });

  web_server.on("/wifi_setting",HTTP_POST,[](AsyncWebServerRequest *request){
    Serial.println("");
    Serial.println("Change wifi setting");

    if(!(request->hasParam("SSID", true))){request->send(200, "text/plain", "Missing data: SSID");}
    if(!(request->hasParam("hidden_SSID", true))){request->send(200, "text/plain", "Missing data: hidden_SSID");}
    if(!(request->hasParam("passwd", true))){request->send(200, "text/plain", "Missing data: password");}
    String SSID = request->getParam("SSID", true)->value();
    String hidden_SSID = request->getParam("hidden_SSID", true)->value();
    String passwd = request->getParam("passwd", true)->value();

    request->send(200, "text/plain", "Restart for the changes to take effect");
    
    Serial.println("SSID:\t"+SSID+"\nhidden_SSID:\t"+hidden_SSID+"\npasswd:\t"+passwd);

    if (SSID == "hidden"){
      Serial.println("hidden SSID");
      SSID=hidden_SSID;
      }
    
    preferences.begin("setting");
    preferences.putString("SSID", SSID);
    preferences.putString("PASSWD", passwd);
    preferences.end();
   });


  web_server.begin();
}

/*
██╗      ██████╗  ██████╗ ██████╗
██║     ██╔═══██╗██╔═══██╗██╔══██╗
██║     ██║   ██║██║   ██║██████╔╝
██║     ██║   ██║██║   ██║██╔═══╝
███████╗╚██████╔╝╚██████╔╝██║
╚══════╝ ╚═════╝  ╚═════╝ ╚═╝

*/
void loop() {
  // put your main code here, to run repeatedly:
  dns_server.processNextRequest();
}