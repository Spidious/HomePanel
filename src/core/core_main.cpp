#include "core/core_main.h"

// This initializes the hardware components
// Display, Touch, Powermanager, (soon to add SD card if initialization is needed)
int core_init()
{
    // Initialize Display Driver
    Serial.println("Initializing display driver...");
    static DisplayDriver displayDriver;
    if (!displayDriver.init()) {
        Serial.println("ERROR: Failed to initialize display!");
        while (1) delay(1000);
    }
    Serial.println("Display driver initialized successfully");

    // Initialize Touch Driver
    Serial.println("Initializing touch driver...");
    static TouchDriver touchDriver;
    if (!touchDriver.init(displayDriver.getLCD())) {
        Serial.println("ERROR: Failed to initialize touch!");
        while (1) delay(1000);
    }
    Serial.println("Touch driver initialized successfully");

    // Initialize Power Manager
    Serial.println("Initializing power manager...");
    PowerManager::init(&displayDriver);
    Serial.println("Power manager initialized successfully");

    return 0;
}