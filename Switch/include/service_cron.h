#ifndef SERVICE_CRON_H
#define SERVICE_CRON_H

#include <CronAlarms.h>
#include <Preferences.h>

extern Preferences preferences;

String cron_key(int num);
void cron_clear();
void cron_load_from_setting();
uint8_t cron_add(String custom_cron); // only accept cron with on/off action
String cron_get_list_json();

#endif