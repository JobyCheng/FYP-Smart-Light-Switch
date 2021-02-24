#include <Arduino.h>

#include <Preferences.h>
#include <HTTPClient.h>

// Library for Network
#include <WiFi.h>
#include "esp_wifi.h"
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

bool isDNSup = false;

enum ROLE{
  SERVER,
  CLIENT
} role;

struct clientList
{
  String id;
  String identifier;
  bool status;
};
const int LIST_SIZE = 16;
clientList CList [LIST_SIZE]; // resize if needed
int CListptr = 0;

void setDefault(){
  Serial.println("Clear all setting");
  preferences.begin("setting");
  preferences.clear();
  preferences.end();
}

void addClient(String id, String identifier = "", bool status = false){
  if (CListptr>=LIST_SIZE){Serial.println("\nMax. No. of client has reached");return;};
  CList[CListptr++] = {id,identifier,status};
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

  // Get all the Needed varible;
  preferences.begin("setting");
  String SSID = preferences.getString("SSID", "");
  String PASSWD = preferences.getString("PASSWD", "");

  String AP_PASSWD = preferences.getString("DEVICE_PASSWD", "12345678");
  preferences.end();

  // product name
  // will be used in name of access point, domain name of site
  String PRODUCT_NAME = "esp32";

  // create a id w/ MAC address.
  Serial.print("Mac address:\t");
  String DEVICE_ID = String((unsigned long) ESP.getEfuseMac(),16);
  Serial.println(DEVICE_ID);

  // add itself to the client list
  // id is also the url for connection, so use product_name as the server id
  addClient(PRODUCT_NAME,"main", false);

  /*
  ███╗   ██╗███████╗████████╗██╗    ██╗ ██████╗ ██████╗ ██╗  ██╗
  ████╗  ██║██╔════╝╚══██╔══╝██║    ██║██╔═══██╗██╔══██╗██║ ██╔╝
  ██╔██╗ ██║█████╗     ██║   ██║ █╗ ██║██║   ██║██████╔╝█████╔╝
  ██║╚██╗██║██╔══╝     ██║   ██║███╗██║██║   ██║██╔══██╗██╔═██╗
  ██║ ╚████║███████╗   ██║   ╚███╔███╔╝╚██████╔╝██║  ██║██║  ██╗
  ╚═╝  ╚═══╝╚══════╝   ╚═╝    ╚══╝╚══╝  ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝

  */
  // Wifi

  if(!(SSID == "")){
    Serial.println("Wifi Conf Found");
    WiFi.begin(SSID.c_str(), PASSWD.c_str());
  }
  // Default as server, change to client if it detect a server.
  role = SERVER;

  if(WiFi.waitForConnectResult()==WL_CONNECTED){
      Serial.println("WiFi:\t\tOK");
      Serial.print("IP address:\t");
      Serial.println(WiFi.localIP());

      // Check if there is any server in the LAN
      HTTPClient http;
      http.begin(PRODUCT_NAME+".local/NewDevice");
      http.addHeader("Content-Type", "application/json");
      // sending the id to server
      String data = "[{id:"+DEVICE_ID+"}]";
      int httpResponseCode = http.POST(data);

      Serial.print("Server Response:\t");
      Serial.println(httpResponseCode);
      if (httpResponseCode == 200){
        role = CLIENT;
      }

      // Power Saving Option
      // lower wifi client power usage
      esp_wifi_set_ps(WIFI_PS_MIN_MODEM);

    }else{
      Serial.println("WiFi:\t\tFail");
  }

  Serial.print("\nRole:\t\t");
  Serial.println((role == CLIENT)? "Client" : "Server");

  if(WiFi.isConnected()){
    // mDNS
    String domain_name = (role == CLIENT)? DEVICE_ID : PRODUCT_NAME;
    if(MDNS.begin(domain_name.c_str())) {
      Serial.println("mDNS:\t\tOK");
      MDNS.addService("http", "tcp", 80);
      Serial.println("address:\thttp://" + domain_name + ".local/");
    }else{
      Serial.println("mDNS:\t\tFail");
    }
  }else{
    //Access point
    IPAddress AP_IP = IPAddress();
    if (WiFi.softAP((PRODUCT_NAME+"-"+DEVICE_ID).c_str())){
      Serial.println("WiFi_AP:\tOK");
      AP_IP = WiFi.softAPIP();
      Serial.println("Name:\t\t"+PRODUCT_NAME);
      Serial.print("AP address:\t");
      Serial.println(AP_IP);
    }else{
      Serial.println("WiFi_AP:\tFail");
    }

    // DNS
    if(dns_server.start(DNS_PORT, "*", AP_IP)){
      Serial.println("DNS:\t\tOK");
      isDNSup = true;
    }else{
      Serial.println("DNS:\t\tFail");
    }
  }


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
    Serial.println("\nReset");
    request->send(200, "text/plain", "Server recived");
    setDefault();
   });

   web_server.on("/restart",HTTP_GET,[](AsyncWebServerRequest *request){
    Serial.println("\nRestart");
    request->send(200, "text/plain", "Server recived");
    ESP.restart();
  });

  web_server.on("/NewDevice",HTTP_GET,[](AsyncWebServerRequest *request){
    Serial.println("\nNewDevice");
    request->send(200, "text/plain", "Server recived");
    //addClient();
  });
  
  web_server.on("/on",HTTP_GET,[](AsyncWebServerRequest *request){
   Serial.println("\nOn");
   request->send(200, "text/plain", "Server recived");
  });
  
  web_server.on("/off",HTTP_GET,[](AsyncWebServerRequest *request){
   Serial.println("\nOff");
   request->send(200, "text/plain", "Server recived");
  });

  web_server.on("/getClient",HTTP_GET,[](AsyncWebServerRequest *request){
    Serial.println("\ngetClient");
    String json = "[";
    for (int i = 0; i < CListptr; ++i){
      if(i) {json += ",";};
      json += "{";
      json += "\"id\":\""+CList[i].id+"\",";
      json += "\"identifier\":\""+CList[i].identifier+"\",";
      json += "\"status\":";
      json += CList[i].status?"true":"false";
      json += "}";
    }
    json += "]";

    request->send(200, "application/json", json);
  });

  web_server.on("/wifiStauts",HTTP_GET,[](AsyncWebServerRequest *request){
    auto status = WiFi.status();
    Serial.println("\nwifiStauts");
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
      Serial.println(status);
      Serial.println(message);

    request->send(200, "text/plain", message);
   });

  web_server.on("/SSIDlist", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("\nSSID");
    int n = WiFi.scanNetworks(); //Not in Async mode
    String json = "[";
    for (int i = 0; i < n; ++i){
      if(i) {json += ",";};
      json += "{";
      json += "\"ssid\":\""+WiFi.SSID(i)+"\"";
      json += "}";
    }
    json += "]";
    request->send(200, "application/json", json);
    WiFi.scanDelete();
  });

  web_server.on("/AP_passwd",HTTP_POST,[](AsyncWebServerRequest *request){
    Serial.println("\nSet Access Point password");
    if(request->hasParam("passwd", true)){
      String passwd = request->getParam("passwd", true)->value();
      Serial.println(passwd);
        preferences.begin("setting");
        preferences.putString("DEVICE_PASSWD",passwd);
        preferences.end();
    }
    request->send(200, "text/plain", "New setting applied, wait until restart");
    ESP.restart();
   });

  web_server.on("/wifi_setting",HTTP_POST,[](AsyncWebServerRequest *request){
    Serial.println("\nChange wifi setting");

    if(!(request->hasParam("SSID", true))){request->send(200, "text/plain", "Missing data: SSID");}
    if(!(request->hasParam("hidden_SSID", true))){request->send(200, "text/plain", "Missing data: hidden_SSID");}
    if(!(request->hasParam("passwd", true))){request->send(200, "text/plain", "Missing data: password");}
    String SSID = request->getParam("SSID", true)->value();
    String hidden_SSID = request->getParam("hidden_SSID", true)->value();
    String passwd = request->getParam("passwd", true)->value();

    request->send(200, "text/plain", "New setting applied, wait until restart");
    
    Serial.println("SSID:\t"+SSID+"\nhidden_SSID:\t"+hidden_SSID+"\npasswd:\t"+passwd);

    if (SSID == "hidden"){
      Serial.println("hidden SSID");
      SSID=hidden_SSID;
    }
    
    preferences.begin("setting");
    preferences.putString("SSID", SSID);
    preferences.putString("PASSWD", passwd);
    preferences.end();
    
    ESP.restart();
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
  if (isDNSup){dns_server.processNextRequest();}
}