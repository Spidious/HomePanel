#include <Arduino.h>
#include <lvgl.h>
#include <WiFi.h>
#include "core/core_main.h"
#include "core/power_manager.h"      // Power Manager module
#include "ui.h"

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n\n=== HomePanel - LVGL 9 with LovyanGFX ===");
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("PSRAM size: %d bytes\n", ESP.getPsramSize());
    Serial.printf("Free PSRAM: %d bytes\n", ESP.getFreePsram());

    // Setup Crowpanel Hardware
    core_init();

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
