#include <Arduino.h>

#include <Preferences.h>
#include <HTTPClient.h>

#include "service_mDNS.h"
#include "service_DNS.h"
#include "service_Wifi_AP.h"
#include "service_time_sync.h"
#include "service_cron.h"
#include "service_Wifi_client.h"

#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

#include <TaskScheduler.h>
#include <CronAlarms.h>

/*
██╗   ██╗ █████╗ ██████╗ ██╗ █████╗ ██████╗ ██╗     ███████╗
██║   ██║██╔══██╗██╔══██╗██║██╔══██╗██╔══██╗██║     ██╔════╝
██║   ██║███████║██████╔╝██║███████║██████╔╝██║     █████╗
╚██╗ ██╔╝██╔══██║██╔══██╗██║██╔══██║██╔══██╗██║     ██╔══╝
 ╚████╔╝ ██║  ██║██║  ██║██║██║  ██║██████╔╝███████╗███████╗
  ╚═══╝  ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝╚═╝  ╚═╝╚═════╝ ╚══════╝╚══════╝

*/
// product name
// will be used in name of access point, domain name of site
String PRODUCT_NAME = "esp32";

AsyncWebServer web_server(80);
Preferences preferences;
DNSServer dns_server;

enum ROLE{SERVER,CLIENT} role;

// Client Storage
struct clientList
{
  String id;
  String identifier;
  bool status;
};
const int LIST_SIZE = 16;
clientList CList [LIST_SIZE]; // resize if needed
int CListptr = 0;

// Task Schedule
Scheduler taskSchedule;
Task t_DNS_request(0, TASK_FOREVER, [](){dns_server.processNextRequest();},&taskSchedule);
Task t_cron(0, TASK_FOREVER, [](){Cron.delay();},&taskSchedule);

/*
███████╗██╗   ██╗███╗   ██╗ ██████╗████████╗██╗ ██████╗ ███╗   ██╗
██╔════╝██║   ██║████╗  ██║██╔════╝╚══██╔══╝██║██╔═══██╗████╗  ██║
█████╗  ██║   ██║██╔██╗ ██║██║        ██║   ██║██║   ██║██╔██╗ ██║
██╔══╝  ██║   ██║██║╚██╗██║██║        ██║   ██║██║   ██║██║╚██╗██║
██║     ╚██████╔╝██║ ╚████║╚██████╗   ██║   ██║╚██████╔╝██║ ╚████║
╚═╝      ╚═════╝ ╚═╝  ╚═══╝ ╚═════╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝

*/

void addClient(String id, String identifier = "", bool status = false){
  // 
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

  // create a id with MAC address.
  Serial.print("Mac address:\t");
  String DEVICE_ID = String((unsigned long) ESP.getEfuseMac(),16);
  Serial.println(DEVICE_ID);

  // Add itself to the client list
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
   // Default as server, change to client if it detect a server.
  role = SERVER;

  // Wifi
  if (wifi_client_start_with_setting()){
    // Check if there is any server in the LAN
    HTTPClient http;
    http.begin(PRODUCT_NAME+".local/NewDevice");
    http.addHeader("Content-Type", "application/json");
    // sending the id to server
    String data = "[{\"id\":"+DEVICE_ID+"}]";
    int httpResponseCode = http.POST(data);
    Serial.print("Server Response:\t");
    Serial.println(httpResponseCode);
    if (httpResponseCode == 200){
      role = CLIENT;
    }
    Serial.println("Role:\t\t"+((role == CLIENT)?String("Client"):String("Server")));

    // mDNS
    mDNS_start((role == CLIENT)? DEVICE_ID : PRODUCT_NAME);

    if(time_sync_start()){  // Start cron if time sync is check.
      cron_load_from_setting();
      t_cron.enable();
    }

  }else{
    //Access point
    wifi_ap_start(PRODUCT_NAME+"-"+DEVICE_ID);
    //DNS
    if(DNS_start("*", WiFi.softAPIP())){t_DNS_request.enable();}
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
    request->send(200);
    Serial.println("Clear all setting");
    preferences.begin("setting");
    preferences.clear();
    preferences.end();
    ESP.restart();
   });

   web_server.on("/restart",HTTP_GET,[](AsyncWebServerRequest *request){
    Serial.println("\nRestart");
    request->send(200);
    ESP.restart();
  });

  web_server.on("/NewDevice",HTTP_GET,[](AsyncWebServerRequest *request){
    Serial.println("\nNewDevice");
    request->send(200);
    //addClient();
  });
  
  web_server.on("/on",HTTP_GET,[](AsyncWebServerRequest *request){
   Serial.println("\nGet:\t\tOn");
   request->send(200);
  });
  
  web_server.on("/off",HTTP_GET,[](AsyncWebServerRequest *request){
   Serial.println("\nGet:\t\tOff");
   request->send(200);
  });

  web_server.on("/getClient",HTTP_GET,[](AsyncWebServerRequest *request){
    Serial.println("\nGet:\t\tClient List");
    String json = "[";
    for (int i = 0; i < CListptr; ++i){
      // Name
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

  web_server.on("/getSchedule",HTTP_GET,[](AsyncWebServerRequest *request){
    Serial.println("\nGet:\t\tGet Schedule");
    preferences.begin("setting");

    String json = "[";
    int i = 0;
    String key = "Schedule_" + String(i);
    while(preferences.isKey(key.c_str())){
      Serial.println("Reading schedule_"+String(i)+"...");
      String data = preferences.getString(key.c_str());
      if(i) {json += ",";};
      json += "{\"value\":\"";
      json += data.c_str();
      json += "\"}";
      key = "Schedule_" + String(++i);
    }
    json += "]";

    preferences.end();
    request->send(200, "application/json", json);
  });

  web_server.on("/setSchedule",HTTP_POST,[](AsyncWebServerRequest *request){
    Serial.println("\nPOST:\t\tSet Schedule");
    preferences.begin("setting");
    cron_clear();

    int i = 0;
    while(request->hasParam(String(i).c_str(), true)){
      String data = request->getParam(String(i).c_str(), true)->value();
      cron_add(data);
      if(!preferences.putString(cron_key(i).c_str(), data)){Serial.println("Fail to add data to key "+ cron_key(i));}
      ++i;
    }
    preferences.end();
    request->send(200);
  });

  web_server.on("/wifiStauts",HTTP_GET,[](AsyncWebServerRequest *request){
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
      Serial.println(status);
      Serial.println(message);

    request->send(200, "text/plain", message);
   });

  // if  a async WiFi search
  web_server.on("/SSIDlist", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("\nGet:\t\tSSID");
    String json = "[";
    int n = WiFi.scanComplete();
    if(n < 0){
      WiFi.scanDelete();
      WiFi.scanNetworks(true);
      request->send(202);
      Serial.println("Respond:\t202");
      return;
    }
    if(n){
      Serial.print("No. of result:\t");
      Serial.println(n);
      for (int i = 0; i < n; ++i){
        Serial.println("\t"+WiFi.SSID(i));
        if(i) {json += ",";};
        json += "{";
        json += "\"ssid\":\""+WiFi.SSID(i)+"\"";
        json += "}";
      }
      WiFi.scanDelete();
      WiFi.scanNetworks(true);
    }
    json += "]";
    Serial.println("Respond:\t200");
    request->send(200, "application/json", json);
  });

  web_server.on("/wifi_setting",HTTP_POST,[](AsyncWebServerRequest *request){
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
    taskSchedule.execute();
}