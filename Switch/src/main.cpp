#include <Arduino.h>
#include <SPIFFS.h>

#include <Preferences.h>
#include <HTTPClient.h>

#include "service_mDNS.h"
#include "service_DNS.h"
#include "service_Wifi_AP.h"
#include "service_time_sync.h"
#include "service_cron.h"
#include "service_Wifi_client.h"

#include <ESPAsyncWebServer.h>
#include "responses.h"

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
std::array<String, LIST_SIZE> list;

String DEVICE_ID;
String LABEL;


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
  DEVICE_ID = String((unsigned long) ESP.getEfuseMac(),16);
  // get label if any
  LABEL = preferences.isKey("LABEL")?preferences.getString("LABEL"):String();
  
  Serial.println("ID:\t\t"+DEVICE_ID);
  Serial.println("label:\t\t"+LABEL);

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
    mDNS_start((role==CLIENT)?DEVICE_ID:PRODUCT_NAME);

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

  web_server.on("/reset",HTTP_GET,responses_reset);
  web_server.on("/restart",HTTP_GET,responses_restart);

  web_server.on("/on",HTTP_GET,responses_on);
  web_server.on("/off",HTTP_GET,responses_off);

  web_server.on("/getSchedule",HTTP_GET,responses_getSchedule);
  web_server.on("/setSchedule",HTTP_POST,responses_setSchedule);

  web_server.on("/wifiStauts",HTTP_GET,responses_wifiStauts);
  web_server.on("/SSIDlist", HTTP_GET,responses_SSIDlist);
  web_server.on("/wifi_setting",HTTP_POST,responses_wifi_setting);

  web_server.on("/NewDevice",HTTP_GET,responses_NewDevice);
  web_server.on("/getClient",HTTP_GET,[](AsyncWebServerRequest *request){
    Serial.println("\nGet:\t\tClient List");
    String json = "[";
    for (int i = 0; i < CListptr; ++i){
      // Name
      if(i) {json += ",";};
      json += "{";
      json += "\"id\":\""+CList[i].id+"\"";
      json += "}";
    }
    json += "]";

    request->send(200, "application/json", json);
  });

  web_server.on("/info",HTTP_GET,[](AsyncWebServerRequest *request){
    Serial.println("\nGet:\t\tInfo");
    String json = "[{";
    json += "\"id\":\""+((role==CLIENT)?DEVICE_ID:PRODUCT_NAME)+"\",";
    json += "\"label\":\""+((LABEL=="")?DEVICE_ID:LABEL)+"\",";
    json += "\"status\":"+String("false"); // NEED TO BE CHANGE!!!!!!!!!!!!!!!!!!!!!!
    json += "}]";
    request->send(200, "application/json", json);
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