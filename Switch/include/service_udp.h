#ifndef SERVICE_UDP_H
#define SERVICE_UDP_H

#include "Arduino.h"
#include "AsyncUDP.h"
#include "WiFiUDP.h"
#include "vector"
#include "struct_client_entry.h"

#define UDP_PORT 8000

extern WiFiUDP udp_sender;
extern AsyncUDP udp_reciver;
extern std::vector<client_entry> client_list;

void onPacketCallBack(AsyncUDPPacket packet);
void udp_server_init();
bool send_udp_message(IPAddress boardcast_IP, String data, int timeout);

#endif