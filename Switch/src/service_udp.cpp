#include "service_udp.h"

void onPacketCallBack(AsyncUDPPacket packet)
{
  Serial.println("\nNew Client");
  String data = String((char *)packet.data()).substring(0,packet.length());
  int seprater = data.indexOf(",");
  String id = data.substring(0,seprater);
  String label = data.substring(seprater+1,data.length());
  Serial.println("ID:\t\t"+id);
  Serial.println("LABEL:\t\t"+label);

  client_list.push_back({id,label});

  AsyncUDPMessage message;
  message.write((uint8_t *)"Recived",8);
  packet.send(message);
}

void udp_server_init(){
  if(!udp_reciver.listen(UDP_PORT)){
    Serial.println("UDP Server:\tFail");
  }
  udp_reciver.onPacket(onPacketCallBack);
  Serial.println("UDP Server:\tOK");
}

bool send_udp_message(IPAddress boardcast_IP, String data, int timeout){
  // Use synchronous method
  udp_sender.begin(UDP_PORT);
  udp_sender.beginPacket(boardcast_IP,8000);
  udp_sender.print(data);
  udp_sender.endPacket();
  unsigned long currentTime = millis();
  while(millis()<currentTime+timeout){
    int packetSize = udp_sender.parsePacket();
    if (packetSize){
      char buf[packetSize];
      udp_sender.read(buf, packetSize);
      String recived_data = String(buf).substring(0,packetSize);
      if (recived_data == "Recived"){
        udp_sender.stop();
        return true;
      }
    }
  }
  return false;
}