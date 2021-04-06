# Smart Light Switch
This is a sum up file. Please update it if anything is implemented or need to be implement.

Run `python localServer.py` to have a look on the web page without any post and get function.

css may not update beacuse of cache. Press `ctrl+F5` for most window browser. `cmd+F5` for mac (not sure)

*avoid using ADC2 (Wifi use ADC2) (not checked) (from https://github.com/khoih-prog/ESPAsync_WiFiManager)*

# Feature

- [x] Access Point (when not connected to a wifi server)
- [x] DNS ( problem exist, solve it later ) (only resolve address with Suffix `.local`(?))
- [x] WIfi client
- [x] mDNS
- [x] Mobile view
- [x] Changing the Access Point password
- [x] Wifi Page ( Fishished the fundimental part, but the UI is bad )
- [x] Reset all the setting ( by entering http://esp32.local/reset with connected device )
- [x] HOME Page
- [x] Multi Switch control 
- [x] time synchronization
- [x] time scheduled task


## Time schedule method
use cron-like syntax
```
second minute hour day month weekday action
```
To simpify the case for user interface, we will change `minute`, `hour` and `action` only.

## AP Sleep
Currently ESP32 AP doesn’t support all of the power save feature defined in Wi-Fi specification. To be specific, the AP only caches unicast data for the stations connect to this AP, but doesn’t cache the multicast data for the stations. If stations connected to the ESP32 AP are power save enabled, they may experience multicast packet loss.

https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/wifi.html?highlight=power%20saving


https://blog.csdn.net/Naisu_kun/article/details/86300456
http://www.iotsharing.com/2017/05/tcp-udp-ip-with-esp32.html
