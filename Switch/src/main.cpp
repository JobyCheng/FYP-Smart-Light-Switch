#include <Arduino.h>

#include <Preferences.h>
#include <HTTPClient.h>
// Network
#include <WiFi.h>
#include "esp_wifi.h"
#include <ESPmDNS.h>
#include <DNSServer.h>
// Webserver
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

enum ROLE{SERVER,CLIENT} role;

AsyncWebServer web_server(80);
Preferences preferences;

// DNS
DNSServer dns_server;
const uint16_t DNS_PORT = 53;

// Time
struct tm timeinfo;

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
void DNS_reqest_callback();
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

void test(){
  if(getLocalTime(&timeinfo)){
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  }
}

void removeAllCron(){
  for(uint8_t id=0;id<Cron.count();++id){
    Cron.free(id);
  }
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
  String SSID = preferences.isKey("SSID")?preferences.getString("SSID"):String();
  String PASSWD = preferences.isKey("PASSWD")?preferences.getString("PASSWD"):String();

  preferences.end();

  // create a id w/ MAC address.
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

  Serial.print("Role:\t\t");
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

    // time
    configTzTime("UTC+8","asia.pool.ntp.org","time.google.com","pool.ntp.org");
    // Can use function like time(), localtime();
    struct tm timeinfo;
    if(getLocalTime(&timeinfo)){
      Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
      t_cron.enable();
    }else{
      Serial.println("Failed to obtain time");
    }

// testing
    String str = String("* 30 6 * * * on");
    bool status = str.endsWith("on");
    String timing = str.substring(0,str.lastIndexOf(' ')-1);
//////////
    int cronid = Cron.create((char *)"*/20 * * * * *", test, false);
    Serial.println(cronid);
    cronid = Cron.create((char *)"*/30 * * * * *", test, false);
    Serial.println(cronid);
    removeAllCron();
    cronid = Cron.create((char *)"*/20 * * * * *", test, false);
    Serial.println(cronid);

  }else{
    //Access point
    IPAddress AP_IP = IPAddress();
    if (WiFi.softAP((PRODUCT_NAME+"-"+DEVICE_ID).c_str())){
      Serial.println("WiFi_AP:\tOK");
      AP_IP = WiFi.softAPIP();
      Serial.println("Name:\t\t"+PRODUCT_NAME+"-"+DEVICE_ID);
      Serial.print("AP address:\t");
      Serial.println(AP_IP);
    }else{
      Serial.println("WiFi_AP:\tFail");
    }
    //DNS
    if(dns_server.start(DNS_PORT, "*", AP_IP)){
      Serial.println("DNS:\t\tOK");
      t_DNS_request.enable();
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
    Serial.println("Clear all setting");
    preferences.begin("setting");
    preferences.clear();
    preferences.end();
    ESP.restart();
   });

   web_server.on("/restart",HTTP_GET,[](AsyncWebServerRequest *request){
    Serial.println("\nRestart");
    request->send(200, "text/plain", "Server recived");
    ESP.restart();
  });

  web_server.on("/NewDevice",HTTP_GET,[](AsyncWebServerRequest *request){
    Serial.println("\nNewDevice");
    request->send(200);
    //addClient();
  });
  
  web_server.on("/on",HTTP_GET,[](AsyncWebServerRequest *request){
   Serial.println("\nGet:\t\tOn");
   request->send(200, "text/plain", "Server recived");
  });
  
  web_server.on("/off",HTTP_GET,[](AsyncWebServerRequest *request){
   Serial.println("\nGet:\t\tOff");
   request->send(200, "text/plain", "Server recived");
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
    Serial.println("\nGet:\t\tSchedule");
    String json = "[";
    json += "{";
    json += "\"value\":\"";
    json += "* 30 6 * * * on";
    json += "\"}";
    json += "]";
    request->send(200, "application/json", json);
  });

  web_server.on("/setSchedule",HTTP_POST,[](AsyncWebServerRequest *request){
    Serial.println("\nPOST:\t\tSchedule");

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