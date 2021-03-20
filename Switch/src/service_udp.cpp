#include "service_udp.h"

bool udp_start(){
  if(!udp.begin(UDP_PORT)){
    Serial.println("UDP listening fail");
    return false;
  }
  Serial.println("UDP listening on Port "+String(UDP_PORT));
  return true;
}

void udp_stop(){
    udp.stop();
}

void udp_boardcast_message(){
  udp.beginPacket(WiFi.broadcastIP(),UDP_PORT);
  udp.print(MAC_ADDR+","+LABEL);
  udp.endPacket();
}

bool udp_checkServer(int timeout){
  // Use synchronous method
  udp_boardcast_message();
  unsigned long currentTime = millis();
  while(millis()<currentTime+timeout){
    int packetSize = udp.parsePacket();
    if (!packetSize){continue;}

    char buf[packetSize];
    udp.read(buf, packetSize);
    String recived_data = String(buf).substring(0,packetSize);
    
    if (recived_data == "Recived"){
      return true;
    }
  }
  return false;
}

void udp_handle_next_packet()
{
  int packetSize = udp.parsePacket();
  if (!packetSize){return;}
  
  Serial.println("\nNew Client");
  char buf[packetSize];
  udp.read(buf, packetSize);
  String recived_data = String(buf).substring(0,packetSize);
  int seprater = recived_data.indexOf(",");
  String id = recived_data.substring(0,seprater);
  String label = recived_data.substring(seprater+1,recived_data.length());
  IPAddress address = udp.remoteIP();
  Serial.println("ID:\t\t"+id);
  Serial.println("LABEL:\t\t"+label);
  Serial.println("ADDRESS:\t"+address.toString());

  // Check before adding to list
  bool existed = false;
  for (int i = 0; i < client_list.size(); ++i){
    if (id == client_list[i].id){
      existed = true;
      client_list[i].label=label;
      client_list[i].address=address;
      break;
    }
  }
  if (!existed){
    client_list.push_back({id,label,address});
  }

  udp.begin(UDP_PORT);
  udp.beginPacket(udp.remoteIP(),udp.remotePort());
  udp.print("Recived");
  udp.endPacket();
}