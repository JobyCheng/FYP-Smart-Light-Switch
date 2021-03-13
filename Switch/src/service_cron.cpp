#include "service_cron.h"

// dummy function will be replace by other later
void turnON(){Serial.println("ON");};
void turnOFF(){Serial.println("OFF");};

String cron_key(int num){return "Schedule_" + String(num);}

void cron_clear(){
    Cron = CronClass();
    int i = 0;
    while(preferences.isKey(cron_key(i).c_str())){
        preferences.remove(cron_key(i).c_str());
        ++i;
    }
}

void cron_load_from_setting(){
    preferences.begin("setting");
    int failed = 0;
    int i = 0;
    while(preferences.isKey(cron_key(i).c_str())){
        String data = preferences.getString(cron_key(i).c_str());
        if(cron_add(data)==255){++failed;};
        ++i;
    }
    Serial.println(String(i-failed)+" cron task loaded, "+String(failed)+" failed");
    preferences.end();
}

uint8_t cron_add(String custom_cron){
    String timing = custom_cron.substring(0,custom_cron.lastIndexOf(' '));
    return Cron.create((char *)timing.c_str(),(custom_cron.endsWith("on")?turnON:turnOFF),false);
}