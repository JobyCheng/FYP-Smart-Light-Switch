#include "service_DNS.h"

bool DNS_start(String domain_name,IPAddress IP){
    if(!dns_server.start(DNS_PORT, domain_name, IP)){
      Serial.println("DNS:\t\tFail");
      return false;
    }
    Serial.println("DNS:\t\tOK");
    return true;
}