#include <Arduino.h>
#include <lvgl.h>
#include <WiFi.h>
#include "core/core_main.h"
#include "core/power_manager.h"      // Power Manager module
#include "ui.h"
#include <LittleFS.h>

void listFS() {
    if(!LittleFS.begin()) {
        Serial.println("Failed to mount FS");
        return;
    }

    File root = LittleFS.open("/assets");
    File file = root.openNextFile();
    while(file) {
        Serial.print("File: ");
        Serial.println(file.name());
        file = root.openNextFile();
    }
}

void setup()
{
    Serial.begin(115200);
    delay(1000);
    // Debug messages
    Serial.println("\n\n=== Crowpanel ===");
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("PSRAM size: %d bytes\n", ESP.getPsramSize());
    Serial.printf("Free PSRAM: %d bytes\n", ESP.getFreePsram());

    if(!LittleFS.begin()) {
        Serial.println("Failed to mount FS");
        return;
    }

    // Setup Crowpanel Hardware
    core_init();

    // Begin the UI
    ui_init();

    listFS();
}

// Main application loop
void loop()
{
    // Grab the current time
    uint32_t currentMillis = millis();

    // Update LVGL tick (CRITICAL for timers and input device polling!)
    static uint32_t lastTick = 0;
    lv_tick_inc(currentMillis - lastTick);
    lastTick = currentMillis;

    // Update power manager with OFFLINE state (treat as IDLE for power management)
    PowerManager::update(0);

    // Let the UI do its thing
    lv_timer_handler();

    delay(5);
}
