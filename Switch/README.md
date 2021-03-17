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


https://blog.csdn.net/Naisu_kun/article/details/86300456
http://www.iotsharing.com/2017/05/tcp-udp-ip-with-esp32.html

client testing node-red
[
    {
        "id": "ebac78e7.1cd328",
        "type": "tab",
        "label": "Flow 1",
        "disabled": false,
        "info": ""
    },
    {
        "id": "84beca8f.4b6038",
        "type": "udp in",
        "z": "ebac78e7.1cd328",
        "name": "",
        "iface": "",
        "port": "8000",
        "ipv": "udp4",
        "multicast": "false",
        "group": "192.168.120.255",
        "datatype": "buffer",
        "x": 400,
        "y": 120,
        "wires": [
            [
                "45ca736d.1fc42c",
                "918d67ce.f06f98"
            ]
        ]
    },
    {
        "id": "6bfd6d14.b13c34",
        "type": "udp out",
        "z": "ebac78e7.1cd328",
        "name": "",
        "addr": "",
        "iface": "",
        "port": "",
        "ipv": "udp4",
        "outport": "",
        "base64": false,
        "multicast": "false",
        "x": 550,
        "y": 340,
        "wires": []
    },
    {
        "id": "45ca736d.1fc42c",
        "type": "change",
        "z": "ebac78e7.1cd328",
        "name": "",
        "rules": [
            {
                "t": "set",
                "p": "payload",
                "pt": "msg",
                "to": "Recived",
                "tot": "str"
            },
            {
                "t": "delete",
                "p": "_msgid",
                "pt": "msg"
            }
        ],
        "action": "",
        "property": "",
        "from": "",
        "to": "",
        "reg": false,
        "x": 400,
        "y": 240,
        "wires": [
            [
                "6bfd6d14.b13c34",
                "95052c9.3c3f2d"
            ]
        ]
    },
    {
        "id": "95052c9.3c3f2d",
        "type": "debug",
        "z": "ebac78e7.1cd328",
        "name": "",
        "active": true,
        "tosidebar": true,
        "console": false,
        "tostatus": false,
        "complete": "true",
        "targetType": "full",
        "statusVal": "",
        "statusType": "auto",
        "x": 570,
        "y": 220,
        "wires": []
    },
    {
        "id": "918d67ce.f06f98",
        "type": "debug",
        "z": "ebac78e7.1cd328",
        "name": "",
        "active": true,
        "tosidebar": true,
        "console": false,
        "tostatus": false,
        "complete": "true",
        "targetType": "full",
        "statusVal": "",
        "statusType": "auto",
        "x": 550,
        "y": 80,
        "wires": []
    }
]

Server testing node red
[
    {
        "id": "ebac78e7.1cd328",
        "type": "tab",
        "label": "Flow 1",
        "disabled": false,
        "info": ""
    },
    {
        "id": "c87b846e.59e1d8",
        "type": "udp in",
        "z": "ebac78e7.1cd328",
        "name": "",
        "iface": "",
        "port": "8000",
        "ipv": "udp4",
        "multicast": "false",
        "group": "",
        "datatype": "buffer",
        "x": 380,
        "y": 380,
        "wires": [
            [
                "c5f00954.8e1f98"
            ]
        ]
    },
    {
        "id": "c5f00954.8e1f98",
        "type": "debug",
        "z": "ebac78e7.1cd328",
        "name": "",
        "active": true,
        "tosidebar": true,
        "console": false,
        "tostatus": false,
        "complete": "payload",
        "targetType": "msg",
        "statusVal": "",
        "statusType": "auto",
        "x": 490,
        "y": 440,
        "wires": []
    },
    {
        "id": "150e9031.f5296",
        "type": "inject",
        "z": "ebac78e7.1cd328",
        "name": "",
        "props": [
            {
                "p": "payload"
            }
        ],
        "repeat": "",
        "crontab": "",
        "once": false,
        "onceDelay": 0.1,
        "topic": "",
        "payload": "efc40a24,testing",
        "payloadType": "str",
        "x": 420,
        "y": 200,
        "wires": [
            [
                "a6b9f8ec.e759e8"
            ]
        ]
    },
    {
        "id": "a6b9f8ec.e759e8",
        "type": "udp out",
        "z": "ebac78e7.1cd328",
        "name": "",
        "addr": "192.168.120.255",
        "iface": "",
        "port": "8000",
        "ipv": "udp4",
        "outport": "",
        "base64": false,
        "multicast": "broad",
        "x": 520,
        "y": 260,
        "wires": []
    }
]