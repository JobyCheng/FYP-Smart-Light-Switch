# Example for WiFi Access Point , DNS, mDNS
Origianl code: https://randomnerdtutorials.com/esp32-web-server-arduino-ide/

The line I added are marked by some comment line.
```
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!start


The added code is here


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!end
```
# To use the code
rename `setting-template.h` to `setting.h` and edit the content in it.

Then compile the `.ino` file.



# Different of DNS and mDNS 

### DNS
DNS is for Access Point mode. it resolve the domain name from client.

### mDNS
mDNS is for client mode. it told the DNS server on LAN that certain domain name should redirect to its IP.