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
 String keyList[]={
  "DEVICE_PASSWD",
  "SSID",
  "PASSWD"
};


void wifi_check(){
  if(WiFi.status() == WL_CONNECTED){
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

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
  Serial.println("Setting AP (Access Point)…");

  String DEVICE_NAME = "esp32";
  String DEVICE_PASSWD = preferences.getString("DEVICE_PASSWD", "12345678");
  Serial.print("The current password is ");
  Serial.println(DEVICE_PASSWD.c_str());

  WiFi.softAP(DEVICE_NAME.c_str(),DEVICE_PASSWD.c_str());
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // DNS
  if(dns_server.start(DNS_PORT, "*", IP)){
     Serial.println("DNS Started");
  }else{
     Serial.println("Fail to start DNS");
  }

  // mDNS
  if(MDNS.begin(DEVICE_NAME.c_str())) {
     Serial.println("mDNS Started");
  }else{
     Serial.println("Fail to start mDNS");
  }

  // Wifi client
  String SSID = preferences.getString("SSID", "");
  String PASSWD = preferences.getString("PASSWD", "");
  Serial.println(SSID.c_str());
  if(!(SSID == "")){
    Serial.println("Wifi setting found!");
    WiFi.begin(SSID.c_str(), PASSWD.c_str());
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
    Serial.println("recive Reset request");
    request->send(200, "text/plain", "Server recived");
    setDefault();
   });

  web_server.on("/AP_passwd",HTTP_POST,[](AsyncWebServerRequest *request){
    Serial.println("recive POST request");
    request->send(200, "text/plain", "Restart for the changes to take effect");
    if(request->hasParam("passwd", true)){
      String passwd = request->getParam("passwd", true)->value();
      Serial.println(passwd);
        preferences.begin("setting");
        preferences.putString("DEVICE_PASSWD",passwd);
        preferences.end();
    }
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