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
WiFiUDP udp;

bool DNS_enabled;

unsigned long lastLoop;

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
  MAC_ADDR = String((unsigned long) ESP.getEfuseMac(),16);
  DEVICE_ID = MAC_ADDR;

  // get label if any
  preferences.begin("setting");
  LABEL = preferences.isKey("LABEL")?preferences.getString("LABEL"):String(DEVICE_ID);
  preferences.end();
  // Default as server, change to client if it detect a server.
  role = SERVER;

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
    if (udp_start()){
      if(udp_checkServer(2000)){
        Serial.println("Server found");
        udp_stop();
        role = CLIENT;
      }else{
        mDNS_start(PRODUCT_NAME);
    }

    Serial.println("Role:\t\t"+((role == CLIENT)?String("Client"):String("Server")));
    Serial.println("ID:\t\t"+DEVICE_ID);
    Serial.println("label:\t\t"+LABEL);
    
    if(time_sync_start()){  // Start cron if time sync is check.
      cron_load_from_setting();
    }
    
  }else{
    //Access point
    wifi_ap_start(PRODUCT_NAME+"-"+DEVICE_ID);
    //DNS
    DNS_enabled = DNS_start("*", WiFi.softAPIP());
    //if(DNS_enabled){t_DNS_request.enable();}
  }

  client_list.push_back({DEVICE_ID,LABEL,WiFi.localIP()});}


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
  
  /*
  ██████╗ ███████╗ ██████╗ ██╗   ██╗███████╗███████╗████████╗
  ██╔══██╗██╔════╝██╔═══██╗██║   ██║██╔════╝██╔════╝╚══██╔══╝
  ██████╔╝█████╗  ██║   ██║██║   ██║█████╗  ███████╗   ██║
  ██╔══██╗██╔══╝  ██║▄▄ ██║██║   ██║██╔══╝  ╚════██║   ██║
  ██║  ██║███████╗╚██████╔╝╚██████╔╝███████╗███████║   ██║
  ╚═╝  ╚═╝╚══════╝ ╚══▀▀═╝  ╚═════╝ ╚══════╝╚══════╝   ╚═╝

  */

 if(role == SERVER){
  web_server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  web_server.on("/wifiStauts",HTTP_GET,responses_wifiStauts);
  web_server.on("/SSIDlist", HTTP_GET,responses_SSIDlist);
  web_server.on("/wifi_setting",HTTP_POST,responses_wifi_setting);
   
  web_server.on("/getClient",HTTP_GET,responses_getClient);
 }

  web_server.on("/reset",HTTP_GET,responses_reset);
  web_server.on("/restart",HTTP_GET,responses_restart);

  web_server.on("/on",HTTP_GET,responses_on);
  web_server.on("/off",HTTP_GET,responses_off);

  web_server.on("/getSchedule",HTTP_GET,responses_getSchedule);
  web_server.on("/setSchedule",HTTP_POST,responses_setSchedule);

  web_server.on("/status",HTTP_GET,responses_status);
  web_server.on("/setLabel",HTTP_GET,responses_setLabel);

  web_server.on("/calibration",HTTP_GET,responses_calibration);  

  web_server.begin();
  
  lastLoop = millis();
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
  Cron.delay();
  if (DNS_enabled){dns_server.processNextRequest();};
  if (role==SERVER){udp_handle_next_packet();}
  if (role==CLIENT && (millis()>lastLoop+1000*60)){udp_boardcast_message();lastLoop=millis();}
}