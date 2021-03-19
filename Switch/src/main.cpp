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
#include "service_udp.h"
#include "struct_client_entry.h"

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

std::vector<client_entry> client_list;

enum ROLE{SERVER,CLIENT} role;
String DEVICE_ID;
String LABEL;
String MAC_ADDR;

AsyncWebServer web_server(80);
Preferences preferences;
DNSServer dns_server;
extern CronClass Cron;
extern WiFiClass WiFi;
WiFiUDP udp_sender;
AsyncUDP udp_reciver;

// Task Schedule
Scheduler taskSchedule;
Task t_DNS_request(1, TASK_FOREVER, [](){dns_server.processNextRequest();},&taskSchedule);
Task t_cron(1, TASK_FOREVER, [](){Cron.delay();},&taskSchedule);


/*
███████╗██╗   ██╗███╗   ██╗ ██████╗████████╗██╗ ██████╗ ███╗   ██╗
██╔════╝██║   ██║████╗  ██║██╔════╝╚══██╔══╝██║██╔═══██╗████╗  ██║
█████╗  ██║   ██║██╔██╗ ██║██║        ██║   ██║██║   ██║██╔██╗ ██║
██╔══╝  ██║   ██║██║╚██╗██║██║        ██║   ██║██║   ██║██║╚██╗██║
██║     ╚██████╔╝██║ ╚████║╚██████╗   ██║   ██║╚██████╔╝██║ ╚████║
╚═╝      ╚═════╝ ╚═╝  ╚═══╝ ╚═════╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝

*/

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

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(EN, OUTPUT);

  touchAttachInterrupt(T8, T8wasActivated, threshold);
  touchAttachInterrupt(T9, T9wasActivated, threshold);

  gyro_init();

  
  // create a id with MAC address.
  MAC_ADDR = String((unsigned long) ESP.getEfuseMac(),16);
  // get label if any
  LABEL = preferences.isKey("LABEL")?preferences.getString("LABEL"):String(MAC_ADDR);
  
  // Default as server, change to client if it detect a server.
  role = SERVER;
  DEVICE_ID = PRODUCT_NAME;

  /*
  ███╗   ██╗███████╗████████╗██╗    ██╗ ██████╗ ██████╗ ██╗  ██╗
  ████╗  ██║██╔════╝╚══██╔══╝██║    ██║██╔═══██╗██╔══██╗██║ ██╔╝
  ██╔██╗ ██║█████╗     ██║   ██║ █╗ ██║██║   ██║██████╔╝█████╔╝
  ██║╚██╗██║██╔══╝     ██║   ██║███╗██║██║   ██║██╔══██╗██╔═██╗
  ██║ ╚████║███████╗   ██║   ╚███╔███╔╝╚██████╔╝██║  ██║██║  ██╗
  ╚═╝  ╚═══╝╚══════╝   ╚═╝    ╚══╝╚══╝  ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝
  */
  

  // Wifi
  if (wifi_client_start_with_setting()){
    // Check if there is any server in the LAN
    if(send_udp_message(MAC_ADDR+","+LABEL, 2000)){
      Serial.println("Server found");
      role = CLIENT;
      DEVICE_ID=MAC_ADDR;
    }

    Serial.println("Role:\t\t"+((role == CLIENT)?String("Client"):String("Server")));
    Serial.println("ID:\t\t"+DEVICE_ID);
    Serial.println("label:\t\t"+LABEL);
    
    // mDNS
    mDNS_start(DEVICE_ID);

    if(time_sync_start()){  // Start cron if time sync is check.
      cron_load_from_setting();
      t_cron.enable();
    }
    
    if(role==SERVER){
      udp_server_init();  //Start udp if it is a server
    }

  }else{
    //Access point
    wifi_ap_start(PRODUCT_NAME+"-"+MAC_ADDR);
    //DNS
    if(DNS_start("*", WiFi.softAPIP())){t_DNS_request.enable();}
  }

  // Add itself to the client list
  // id is also the url for connection, so use product_name as the server id
  if (role == SERVER){  // client do not need to use client_list
    client_list.push_back({DEVICE_ID,LABEL});
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

  web_server.on("/getClient",HTTP_GET,responses_getClient);

  web_server.on("/info",HTTP_GET,responses_info);
  web_server.on("/setLabel",HTTP_GET,responses_setLabel);

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