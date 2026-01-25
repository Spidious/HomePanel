#ifndef WIFI_DRIVER_H
#define WIFI_DRIVER_H

#include <WiFi.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

class WiFiDriver
{
private:
    static const char* ssid;
    static const char* passwd;

public:
    WiFiDriver() = delete;
    static void init(StaticJsonDocument<512> jsonData);
};

#endif // WIFI_DRIVER_H
