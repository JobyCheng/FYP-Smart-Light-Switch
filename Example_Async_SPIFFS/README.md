# Example for Async Web Server w/ SPIFFS

*Notice: This example is built using platfromIO, it can tranfer back to `.ino`, but not introduce here.*

This project show how to use a async server to host a website w/ html, javascipt and css.

***Some fundemental function are partical implenment in this example***

# Three item showing in this example

## Hosting a static site(?)

Hosting html,css,js at the same time.

## GET request

It host a website with button "scan"

By clicking "scan" in client, it will send a get request to server.

Server (a.k.a. esp32) will seach for the non-hidden wifi and send a json with some wifi-related data to the client.

Once client recive the data, it will print out all the ssid from json.

## POST request

It will submit a form to the server.

Server will print out the two varible and give respond.

Once client recive the respond, it will print out respond in console.
Can be change to updating DOM object.

# Pros. of this approch
-  More simple program
-  Better structure when there is different tabs
-  Async
-  easy to debug

# Cons.
-  Async can be harder to write
-  Need a well planning


# SPIFFS uploading tutorial
ArdunioIDE: https://randomnerdtutorials.com/install-esp32-filesystem-uploader-arduino-ide/

PlatformIO: https://randomnerdtutorials.com/esp32-vs-code-platformio-spiffs/

# Async web server
Library Used: https://github.com/me-no-dev/ESPAsyncWebServer

** For ArdunioIDE, library need to be installed manualy **
