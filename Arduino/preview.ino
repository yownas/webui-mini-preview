#include <Arduino.h>
#include <HTTPClient.h>
#include <M5Stack.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <string.h>
#include <Base64.h>

#define MY_SSID "WiFi SSID"
#define MY_PASS "WiFi password"
#define PREVIEWURL "http://192.168.1.10:7860/minip/run"

WiFiMulti wifiMulti;

unsigned char bitmap[32768];
            
void setup() {
    M5.begin();        // Init M5Core.
    M5.Power.begin();  // Init power
    wifiMulti.addAP(MY_SSID, MY_PASS);  // Storage wifi configuration information.
}

void loop() {
    String payload;
    int sleep;
    int progress;
    char *indata;
    M5.Lcd.setCursor(0, 0);  // Set the cursor at (0,0).
    if ((wifiMulti.run() ==
         WL_CONNECTED)) {  // wait for WiFi connection.
        Serial.print("[HTTP] begin...\n");
        HTTPClient http;
        http.begin(PREVIEWURL);  // configure traged server and url.  
        int httpCode = http.GET();  // start connection and send HTTP header.
        if (httpCode > 0) {  // httpCode will be negative on error
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);
            if (httpCode == HTTP_CODE_OK) {  // file found at server.
                payload = http.getString();
            }
        } else {
            M5.Lcd.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
            delay(10000);
        }
        // Get progress
        indata = (char *)strstr(payload.c_str(), "progress");
        indata += 10; // length of "progress" + 2
        progress = 0;
        while (indata[0] != ',') {
            progress *= 10;
            progress += indata[0] - 48;
            indata += 1;
        }
        // find start of "current_image" in payload
        indata = (char *)strstr(payload.c_str(), "current_image");
        indata += 15; // length of fieldname + 2
        if (indata[0] == '\"') {
            indata += 1;          
            Base64.decode((char *)bitmap, indata, 40960);          
        } else {
          progress = 0;
        }
        http.end();

        if (progress > 0) {
            M5.Lcd.progressBar(0, 0, 320, 8, progress);
            M5.Lcd.drawJpg(bitmap, 32768, 0, 8);
            sleep = 500;
        } else {
            M5.Lcd.clear();
            sleep = 2000;
        }
    } else {
        M5.Lcd.print("connect failed");
        sleep = 10000;
    }
    delay(sleep);
}
