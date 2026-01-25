#include "core/wifi_driver.h"

void WiFiDriver::init(StaticJsonDocument<512> jsonDoc)
{

    // assign ssid and passwd
    ssid = jsonDoc["ssid"];
    passwd = jsonDoc["passwd"];

    // Connect to wifi as station
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, passwd);

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();

}