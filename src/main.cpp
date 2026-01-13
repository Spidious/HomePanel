#include <Arduino.h>
#include <lvgl.h>
#include <WiFi.h>
#include <Preferences.h>
#include "core/display_driver.h"     // Display driver module
#include "core/power_manager.h"      // Power Manager module
#include "core/touch_driver.h"       // Touch driver module
#include "ui/ui.h"

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n\n=== HomePanel - LVGL 9 with LovyanGFX ===");
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("PSRAM size: %d bytes\n", ESP.getPsramSize());
    Serial.printf("Free PSRAM: %d bytes\n", ESP.getFreePsram());

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

    ui_init();
}

void loop()
{
    uint32_t currentMillis = millis();

    // Update LVGL tick (CRITICAL for timers and input device polling!)
    static uint32_t lastTick = 0;
    lv_tick_inc(currentMillis - lastTick);
    lastTick = currentMillis;


    // Update power manager with OFFLINE state (treat as IDLE for power management)
    PowerManager::update(0);

    lv_timer_handler();
    delay(5);
}
