#ifndef WIFI_DRIVER_H
#define WIFI_DRIVER_H

#include <WiFi.h>

class WiFiDriver
{
private:
    static std::string ssid;
    static std::string passwd;
public:
    WiFiDriver() = delete;
    static void init(void);
};

#endif // WIFI_DRIVER_H
