#include "service_mDNS.h"

bool mDNS_start(String domain_name){
    if(!MDNS.begin(domain_name.c_str())){
        Serial.println("mDNS:\t\tFail");
        return false;
    }
    
    Serial.println("mDNS:\t\tOK");
    MDNS.addService("http", "tcp", 80);
    Serial.println("address:\thttp://" + domain_name + ".local/");
    return true;
}