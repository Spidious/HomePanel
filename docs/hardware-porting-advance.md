# Hardware Porting: CrowPanel Advance 7" Support

## Hardware Comparison

### Current: CrowPanel 7" Basic
- **MCU**: ESP32-S3-WROOM-1-N4R8
  - Flash: 4MB
  - PSRAM: 8MB (Octal)
- **Display**: 800×480 RGB parallel (ST7277 driver in documentation, but using RGB interface)
- **Touch**: GT911 I2C (address 0x5D)
- **Version**: Basic series (no version numbers in documentation)

### Target: CrowPanel 7" Advance
- **MCU**: ESP32-S3-WROOM-1-N16R8
  - Flash: 16MB (4x larger!)
  - PSRAM: 8MB (Octal, same)
- **Display**: 800×480 RGB parallel (ST7277 driver IC)
- **Touch**: GT911 I2C (address 0x5D, same)
- **Version**: Hardware v1.3 (latest as of wiki documentation)

## Key Differences

### 1. Flash Memory (CRITICAL)
- **Basic**: 4MB (`board = esp32-s3-devkitc-1`)
- **Advance**: 16MB (requires custom board definition)
- **Impact**: Larger OTA partitions, more space for fonts/assets, full terminal history

### 2. GPIO Pin Assignments (DIFFERENT!)

Comparing the Basic (our current code) vs Advance (Elecrow example):

| Signal | Basic (Current) | Advance (Elecrow) | Changed? |
|--------|----------------|-------------------|----------|
| **RGB Data Pins** |
| D0 (B0) | GPIO_NUM_11 | GPIO_NUM_15 | ✅ **YES** |
| D1 (B1) | GPIO_NUM_12 | GPIO_NUM_7 | ✅ **YES** |
| D2 (B2) | GPIO_NUM_13 | GPIO_NUM_6 | ✅ **YES** |
| D3 (B3) | GPIO_NUM_14 | GPIO_NUM_5 | ✅ **YES** |
| D4 (B4) | GPIO_NUM_21 | GPIO_NUM_4 | ✅ **YES** |
| D5 (G0) | GPIO_NUM_47 | GPIO_NUM_9 | ✅ **YES** |
| D6 (G1) | GPIO_NUM_48 | GPIO_NUM_46 | ✅ **YES** |
| D7 (G2) | GPIO_NUM_45 | GPIO_NUM_3 | ✅ **YES** |
| D8 (G3) | GPIO_NUM_38 | GPIO_NUM_8 | ✅ **YES** |
| D9 (G4) | GPIO_NUM_39 | GPIO_NUM_16 | ✅ **YES** |
| D10 (G5) | GPIO_NUM_40 | GPIO_NUM_1 | ✅ **YES** |
| D11 (R0) | GPIO_NUM_41 | GPIO_NUM_14 | ✅ **YES** |
| D12 (R1) | GPIO_NUM_42 | GPIO_NUM_21 | ✅ **YES** |
| D13 (R2) | GPIO_NUM_2 | GPIO_NUM_47 | ✅ **YES** |
| D14 (R3) | GPIO_NUM_46 | GPIO_NUM_48 | ✅ **YES** |
| D15 (R4) | GPIO_NUM_3 | GPIO_NUM_45 | ✅ **YES** |
| **Control Pins** |
| HSYNC | GPIO_NUM_8 | GPIO_NUM_39 | ✅ **YES** |
| VSYNC | GPIO_NUM_9 | GPIO_NUM_40 | ✅ **YES** |
| HENABLE (DE) | GPIO_NUM_10 | GPIO_NUM_41 | ✅ **YES** |
| PCLK | GPIO_NUM_1 | GPIO_NUM_0 | ✅ **YES** |
| Backlight | GPIO_NUM_1 | GPIO_NUM_2 | ✅ **YES** |
| **Touch I2C** |
| SDA | GPIO_NUM_19 | GPIO_NUM_19 | ✅ **SAME** |
| SCL | GPIO_NUM_20 | GPIO_NUM_20 | ✅ **SAME** |
| INT | GPIO_NUM_40 | (not used) | N/A |
| RST | GPIO_NUM_38 | GPIO_NUM_38 | ✅ **SAME** |
| I2C Address | 0x5D | 0x5D | ✅ **SAME** |

**IMPORTANT**: Almost ALL display pins are different! Only touch I2C pins remain the same.

### 3. Backlight Control (CHANGED in v1.3)
**Basic**: Direct GPIO control (GPIO_NUM_1)
```cpp
pinMode(BACKLIGHT_PIN, OUTPUT);
digitalWrite(BACKLIGHT_PIN, HIGH); // On
```

**Advance v1.3**: STC8H1K28 microcontroller via I2C (address 0x30)
```cpp
// Brightness: 0 (max) to 244 (min), 245 (off)
Wire.beginTransmission(0x30);
Wire.write(0);      // 0 = maximum brightness
Wire.endTransmission();

// Buzzer control (added in v1.3)
Wire.write(246);    // Buzzer ON
Wire.write(247);    // Buzzer OFF
```

### 4. Additional Hardware (Advance only)
- **Buzzer**: Controlled via STC8H1K28 (P2.7)
- **Microphone**: LMD3526B261-OFA01 on MIC_SD(IO20), MIC_CLK(IO19)
- **Function switches**: DIP switches for MIC&SPK, Wireless Module, or MIC&TF Card modes
- **Wireless module**: Optional expansion for Zigbee/LoRa/NF2401/Matter/Thread

## Implementation Strategy

### Multi-Environment Approach

Create separate PlatformIO environments for each hardware variant:

```ini
[platformio]
default_envs = elecrow-crowpanel-7-basic

[env:elecrow-crowpanel-7-basic]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino
board_build.partitions = default_4MB.csv
board_build.arduino.memory_type = dio_opi
build_flags = 
    ${common.build_flags}
    -D HARDWARE_BASIC
lib_deps = ${common.lib_deps}

[env:elecrow-crowpanel-7-advance]
platform = espressif32
board = esp32-s3-crowpanel-7-advance  ; Custom board definition needed
framework = arduino
board_build.partitions = default_16MB.csv  ; Take advantage of extra flash!
board_build.arduino.memory_type = dio_opi
build_flags = 
    ${common.build_flags}
    -D HARDWARE_ADVANCE
lib_deps = ${common.lib_deps}

[common]
build_flags = 
    -D CORE_DEBUG_LEVEL=0
    -D LV_CONF_INCLUDE_SIMPLE
    -D ARDUINO_USB_CDC_ON_BOOT=1
lib_deps = 
    lovyan03/LovyanGFX@^1.2.7
    lvgl/lvgl@^9.4.0
    tamctec/TAMC_GT911@^1.0.2
    links2004/WebSockets@^2.5.4
```

### Custom Board Definition

Create `boards/esp32-s3-crowpanel-7-advance.json`:

```json
{
  "build": {
    "arduino": {
      "ldscript": "esp32s3_out.ld",
      "memory_type": "dio_opi",
      "partitions": "default_16MB.csv"
    },
    "core": "esp32",
    "extra_flags": [
      "-DARDUINO_ESP32S3_DEV",
      "-DARDUINO_USB_CDC_ON_BOOT=1",
      "-DBOARD_HAS_PSRAM"
    ],
    "f_cpu": "240000000L",
    "f_flash": "80000000L",
    "flash_mode": "dio",
    "hwids": [
      ["0x303A", "0x1001"]
    ],
    "mcu": "esp32s3",
    "variant": "esp32s3"
  },
  "connectivity": [
    "wifi",
    "bluetooth"
  ],
  "debug": {
    "openocd_target": "esp32s3.cfg"
  },
  "frameworks": [
    "arduino",
    "espidf"
  ],
  "name": "Elecrow CrowPanel Advance 7.0\" (16MB Flash, 8MB PSRAM)",
  "upload": {
    "flash_size": "16MB",
    "maximum_ram_size": 327680,
    "maximum_size": 16777216,
    "require_upload_port": true,
    "speed": 921600
  },
  "url": "https://www.elecrow.com/crowpanel-advance-7-0-hmi-esp32-ai-display.html",
  "vendor": "Elecrow"
}
```

### 16MB Partition Table

Create `default_16MB.csv`:

```csv
# Name,   Type, SubType, Offset,  Size, Flags
nvs,      data, nvs,     0x9000,  0x5000,
otadata,  data, ota,     0xe000,  0x2000,
app0,     app,  ota_0,   0x10000, 0x500000,
app1,     app,  ota_1,   0x510000,0x500000,
spiffs,   data, spiffs,  0xA10000,0x5F0000,
```

This gives you:
- 5MB per OTA partition (vs 1.5MB on Basic!)
- 6MB SPIFFS for fonts, assets, config files
- Plenty of room for future expansion

### Code Changes

#### 1. Config Header (include/config.h)

```cpp
// Hardware detection
#if defined(HARDWARE_ADVANCE)
  #define HARDWARE_VERSION "Advance v1.3"
  #define BACKLIGHT_CONTROL_I2C  // Use I2C microcontroller for backlight
#elif defined(HARDWARE_BASIC)
  #define HARDWARE_VERSION "Basic"
  #define BACKLIGHT_CONTROL_GPIO // Use direct GPIO for backlight
#else
  #error "Hardware variant not defined! Use -D HARDWARE_BASIC or -D HARDWARE_ADVANCE"
#endif

// Display pins (DIFFERENT for each hardware)
#if defined(HARDWARE_ADVANCE)
  // Advance v1.3 GPIO assignments
  #define TFT_PIN_D0   15  // B0
  #define TFT_PIN_D1   7   // B1
  #define TFT_PIN_D2   6   // B2
  #define TFT_PIN_D3   5   // B3
  #define TFT_PIN_D4   4   // B4
  #define TFT_PIN_D5   9   // G0
  #define TFT_PIN_D6   46  // G1
  #define TFT_PIN_D7   3   // G2
  #define TFT_PIN_D8   8   // G3
  #define TFT_PIN_D9   16  // G4
  #define TFT_PIN_D10  1   // G5
  #define TFT_PIN_D11  14  // R0
  #define TFT_PIN_D12  21  // R1
  #define TFT_PIN_D13  47  // R2
  #define TFT_PIN_D14  48  // R3
  #define TFT_PIN_D15  45  // R4
  #define TFT_PIN_HSYNC   39
  #define TFT_PIN_VSYNC   40
  #define TFT_PIN_HENABLE 41
  #define TFT_PIN_PCLK    0
  #define TFT_BACKLIGHT_PIN 2  // Controlled via STC8H1K28
#else
  // Basic GPIO assignments (current)
  #define TFT_PIN_D0   11  // B0
  #define TFT_PIN_D1   12  // B1
  #define TFT_PIN_D2   13  // B2
  // ... (rest as current)
  #define TFT_BACKLIGHT_PIN 1
#endif

// Touch pins (SAME for both)
#define TOUCH_SDA 19
#define TOUCH_SCL 20
#define TOUCH_RST 38
#define TOUCH_INT 40  // Only used on Basic
#define TOUCH_I2C_ADDR 0x5D
```

#### 2. Display Driver (src/core/display_driver.cpp)

```cpp
void DisplayDriver::init() {
    auto cfg = _bus_instance.config();
    cfg.panel = &_panel_instance;
    
    // Use config.h definitions (which are hardware-specific)
    cfg.pin_d0  = GPIO_NUM(TFT_PIN_D0);
    cfg.pin_d1  = GPIO_NUM(TFT_PIN_D1);
    // ... (all other pins from config.h)
    
    cfg.pin_henable = GPIO_NUM(TFT_PIN_HENABLE);
    cfg.pin_vsync   = GPIO_NUM(TFT_PIN_VSYNC);
    cfg.pin_hsync   = GPIO_NUM(TFT_PIN_HSYNC);
    cfg.pin_pclk    = GPIO_NUM(TFT_PIN_PCLK);
    cfg.freq_write  = 15000000;
    
    // Timing parameters (SAME for both according to wiki)
    cfg.hsync_polarity    = 0;
    cfg.hsync_front_porch = 40;
    cfg.hsync_pulse_width = 48;
    cfg.hsync_back_porch  = 40;
    cfg.vsync_polarity    = 0;
    cfg.vsync_front_porch = 1;
    cfg.vsync_pulse_width = 31;
    cfg.vsync_back_porch  = 13;
    cfg.pclk_active_neg   = 1;
    cfg.de_idle_high      = 0;
    cfg.pclk_idle_high    = 0;
    
    _bus_instance.config(cfg);
    
    // Backlight initialization
    #if defined(BACKLIGHT_CONTROL_I2C)
        // Advance: Initialize STC8H1K28 microcontroller
        Wire.begin(TOUCH_SDA, TOUCH_SCL);
        setBacklightBrightness(0); // 0 = max brightness
    #else
        // Basic: Direct GPIO control
        pinMode(TFT_BACKLIGHT_PIN, OUTPUT);
        digitalWrite(TFT_BACKLIGHT_PIN, HIGH);
    #endif
    
    Serial.printf("Display initialized: %s\n", HARDWARE_VERSION);
}

void DisplayDriver::setBacklightBrightness(uint8_t level) {
    #if defined(BACKLIGHT_CONTROL_I2C)
        // Advance v1.3: 0-244 brightness, 245 = off
        Wire.beginTransmission(0x30);
        Wire.write(level);
        Wire.endTransmission();
    #else
        // Basic: On/Off only
        digitalWrite(TFT_BACKLIGHT_PIN, level > 0 ? HIGH : LOW);
    #endif
}
```

#### 3. Touch Driver (src/core/touch_driver.cpp)

No changes needed! Touch I2C pins and address are the same on both hardware variants.

#### 4. Main Application (src/main.cpp)

```cpp
void setup() {
    Serial.begin(115200);
    Serial.printf("\n\nFluidTouch %s\n", FLUIDTOUCH_VERSION);
    Serial.printf("Hardware: %s\n", HARDWARE_VERSION);
    Serial.printf("Flash: %d MB, PSRAM: %d MB\n", 
        ESP.getFlashChipSize() / (1024 * 1024),
        ESP.getPsramSize() / (1024 * 1024));
    
    DisplayDriver::init();
    TouchDriver::init();
    // ... rest of initialization
}
```

## Testing Procedure

### When You Receive the Advance Hardware:

1. **Extract Pin Configuration from Example**:
   ```bash
   # Compare their display_driver with ours
   grep -A 30 "cfg.pin_d" main.cpp
   ```

2. **Verify I2C Addresses**:
   ```cpp
   // Run I2C scanner to confirm:
   // - Touch: 0x5D
   // - Backlight controller: 0x30
   ```

3. **Test Build for Both Environments**:
   ```powershell
   # Build Basic (current hardware)
   & "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" run -e elecrow-crowpanel-7-basic
   
   # Build Advance (new hardware)
   & "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" run -e elecrow-crowpanel-7-advance --target upload
   ```

4. **Verification Checklist**:
   - [ ] Display shows correctly (no corruption, correct colors)
   - [ ] Touch responds accurately to all screen areas
   - [ ] Backlight brightness control works (I2C on Advance)
   - [ ] WiFi connects successfully
   - [ ] Serial debugging output shows correct hardware version
   - [ ] OTA updates work with larger partition
   - [ ] Screenshot server works
   - [ ] FluidNC WebSocket connection successful

## Benefits of 16MB Flash

With 4x the flash memory, you can:

1. **Larger OTA Partitions**: 5MB per OTA slot (vs 1.5MB) = more features without worrying about size
2. **Full Terminal History**: Store thousands of lines instead of current 8KB buffer
3. **Multiple Fonts**: Add more font sizes and styles without compromising features
4. **Local File Storage**: Store GCode files, configuration backups, logs
5. **Future Expansion**: Room for:
   - Web-based configuration interface
   - Macro library
   - Machine profiles
   - Touch calibration data
   - Network logs

## Migration Path

### Phase 1: Preparation (Before Hardware Arrives)
- ✅ Document pin differences (this file)
- ✅ Create conditional compilation structure in config.h
- Create custom board definition JSON
- Create 16MB partition table CSV
- Update platformio.ini with both environments

### Phase 2: Initial Testing (Hardware Arrives)
- Build and flash Advance firmware
- Verify basic display functionality
- Test touch calibration
- Verify backlight I2C control
- Check WiFi/Bluetooth functionality

### Phase 3: Integration
- Test all FluidTouch features on Advance
- Verify OTA updates work
- Performance comparison vs Basic
- Update documentation with findings

### Phase 4: Release
- Update main README with hardware compatibility list
- Add hardware selection guide for users
- Publish both firmware variants in releases
- Update copilot instructions with Advance details

## References

- **Elecrow Wiki (Advance v1.3)**: https://www.elecrow.com/pub/wiki/ESP32_Display-7.0_inch%28Advance_Series%29wiki.html
- **Elecrow Example Code**: `C:\Users\jeyea\Downloads\202506\CrowPanel_ESP32_7.0\`
- **GitHub Repository**: https://github.com/Elecrow-RD/CrowPanel-Advance-7-HMI-ESP32-S3-AI-Powered-IPS-Touch-Screen-800x480
- **Product Page**: https://www.elecrow.com/crowpanel-advance-7-0-hmi-esp32-ai-display-800x480-artificial-intelligent-ips-touch-screen-support-meshtastic-and-arduino-lvgl-micropython.html

## Notes

- The Advance version has **significantly different GPIO assignments** for the RGB display
- Touch controller pins are **identical** (SDA/SCL/RST/Address)
- Backlight control changed from GPIO to I2C microcontroller in v1.3
- Same PSRAM (8MB), same display resolution (800×480)
- Main benefit: **16MB flash** enables much larger applications and features
- Conditional compilation keeps codebase maintainable for both variants
