#ifndef SERVICE_DNS_H
#define SERVICE_DNS_H

#include <DNSServer.h>

#define DNS_PORT 53

extern DNSServer dns_server;

bool DNS_start(String domain_name,IPAddress IP);

#endif