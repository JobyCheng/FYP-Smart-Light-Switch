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
- [ ] HOME Page
- [ ] Multi Switch control (part of it finished, need testing and improvement)
- [ ] time scheduled task
- [x] time synchronization

## Multi Switch control Detail
There are a two case:
- boot at different time
- boot at the same time

Flow (theoratical):

Send a request to `esp32.local/something` to check if it exist

if there no respond, open a server with client and end

post the ip and mac address of client to the server

server save the mac & ip. Then assign a id to it.<br>*id and mac need to be save permanently.*

    Example `ip[mac]`&`mac[id]`  or  json?

Function that need to be a request:
- turn on
- turn off
- calibration

## Role:
To determent if it is a server or not, check the mDNS address is occupied or not after connect to wifi.
If it did not join a wifi network. Assume it is a server.
### Diff:
||Server|Client|
|---|---|---|
|mDNS address|use default name|use id|

---

## Time schedule method
use cron-like syntax
```
second minute hour day month weekday action
```
To simpify the case for user interface, we will change `minute`, `hour` and `action` only.

## AP Sleep
Currently ESP32 AP doesn’t support all of the power save feature defined in Wi-Fi specification. To be specific, the AP only caches unicast data for the stations connect to this AP, but doesn’t cache the multicast data for the stations. If stations connected to the ESP32 AP are power save enabled, they may experience multicast packet loss.

https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/wifi.html?highlight=power%20saving
