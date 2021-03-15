#include "service_time_sync.h"

bool time_sync_start(){
    // time "UTC-8"== UTC+8, check out
    // https://unix.stackexchange.com/questions/104088/why-does-tz-utc-8-produce-dates-that-are-utc8
    configTzTime("UTC-8","asia.pool.ntp.org","time.google.com","pool.ntp.org");

    // Check if it is working or not
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
        Serial.println("Failed to obtain time");
        return false;
    }
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    return true;
}