#ifndef SERVICE_UDP_H
#define SERVICE_UDP_H

#include "Arduino.h"
#include "WiFiUDP.h"
#include "WiFi.h"
#include "vector"
#include "struct_client_entry.h"

#define UDP_PORT 8000

extern WiFiUDP udp;
extern std::vector<client_entry> client_list;
extern String LABEL;
extern String MAC_ADDR;

bool udp_start();
void udp_stop();
bool udp_checkServer(int timeout);
void udp_boardcast_message();
void udp_handle_next_packet();

#endif