# Web Installer Multi-Hardware Support

## Overview

This document outlines the strategy for updating the FluidTouch web installer to support both CrowPanel 7" Basic and Advance hardware variants through a single, user-friendly interface.

## Current State (v0.9.1)

- **Single hardware support**: CrowPanel 7" Basic only
- **Web installer**: ESP Web Tools with single manifest.json
- **Firmware path**: `web/firmware/*.bin` (single build)
- **GitHub Actions**: Builds one environment (`elecrow-crowpanel-7-basic`)

## Target State (v0.10.0+)

- **Multi-hardware support**: Both Basic and Advance variants
- **Unified installer**: Single page with hardware selection UI
- **Firmware structure**: `web/firmware/basic/*.bin` and `web/firmware/advance/*.bin`
- **GitHub Actions**: Builds both environments in parallel

---

## Implementation Strategy

### Option 1: Hardware Selection UI (RECOMMENDED)

**Architecture**: Single web page with pre-install hardware selection.

**Advantages**:
- Professional user experience with hardware comparison
- Single URL to share (better for documentation and support)
- ESP Web Tools natively supports multiple builds
- Prevents user confusion from multiple installer pages
- Shows hardware differences before installation

**User Flow**:
1. User visits installer page
2. Sees side-by-side hardware comparison cards
3. Selects matching hardware variant (Basic or Advance)
4. Confirmation message shows selected hardware
5. Clicks install button (ESP Web Tools handles build selection)
6. Firmware flashes appropriate variant

---

## Technical Implementation

### 1. Manifest.json Structure

ESP Web Tools supports multiple builds in a single manifest through the `builds` array. Each build gets a name shown in the installer UI.

```json
{
  "name": "FluidTouch",
  "version": "0.10.0",
  "home_assistant_domain": "esphome",
  "new_install_prompt_erase": true,
  "funding_url": "https://github.com/jeyeager65/FluidTouch",
  "builds": [
    {
      "chipFamily": "ESP32-S3",
      "name": "CrowPanel 7\" Basic (4MB Flash)",
      "parts": [
        { "path": "firmware/basic/bootloader.bin", "offset": 0 },
        { "path": "firmware/basic/partitions.bin", "offset": 32768 },
        { "path": "firmware/basic/boot_app0.bin", "offset": 57344 },
        { "path": "firmware/basic/firmware.bin", "offset": 65536 }
      ]
    },
    {
      "chipFamily": "ESP32-S3",
      "name": "CrowPanel 7\" Advance (16MB Flash)",
      "parts": [
        { "path": "firmware/advance/bootloader.bin", "offset": 0 },
        { "path": "firmware/advance/partitions.bin", "offset": 32768 },
        { "path": "firmware/advance/boot_app0.bin", "offset": 57344 },
        { "path": "firmware/advance/firmware.bin", "offset": 65536 }
      ]
    }
  ]
}
```

**Key Points**:
- `name` field in each build appears in ESP Web Tools dropdown
- Same `chipFamily` for both (ESP32-S3)
- Different firmware paths for each variant
- Flash offsets remain identical (standard ESP32-S3 layout)

### 2. Directory Structure

```
web/
├── index.html              # Main installer page with hardware selection
├── manifest.json           # Multi-build manifest
├── README.md              # Web installer documentation
└── firmware/
    ├── basic/             # Basic model binaries
    │   ├── bootloader.bin
    │   ├── partitions.bin
    │   ├── boot_app0.bin
    │   └── firmware.bin
    └── advance/           # Advance model binaries
        ├── bootloader.bin
        ├── partitions.bin
        ├── boot_app0.bin
        └── firmware.bin
```

### 3. HTML Structure (index.html)

**Hardware Selection Section** (before install button):

```html
<div class="hardware-selection">
    <h2>Select Your Hardware</h2>
    <p class="selection-subtitle">Choose the model that matches your CrowPanel display</p>
    
    <div class="hardware-cards">
        <!-- Basic Model Card -->
        <div class="hardware-card selected" data-build="0">
            <div class="card-header">
                <h3>CrowPanel 7" Basic</h3>
                <div class="default-badge">Most Common</div>
            </div>
            
            <div class="chip-info">ESP32-S3-WROOM-1-N4R8</div>
            
            <div class="specs-grid">
                <div class="spec-item">
                    <span class="spec-label">Flash Memory</span>
                    <span class="spec-value">4MB</span>
                </div>
                <div class="spec-item">
                    <span class="spec-label">PSRAM</span>
                    <span class="spec-value">8MB</span>
                </div>
                <div class="spec-item">
                    <span class="spec-label">OTA Partition</span>
                    <span class="spec-value">1.5MB per slot</span>
                </div>
                <div class="spec-item">
                    <span class="spec-label">File Storage</span>
                    <span class="spec-value">~200KB</span>
                </div>
            </div>
            
            <div class="card-footer">
                <a href="https://www.elecrow.com/esp32-display-7-inch-hmi-display-rgb-tft-lcd-touch-screen-support-lvgl.html" 
                   target="_blank" class="product-link">View Product Page →</a>
            </div>
        </div>
        
        <!-- Advance Model Card -->
        <div class="hardware-card" data-build="1">
            <div class="card-header">
                <h3>CrowPanel 7" Advance</h3>
                <div class="upgrade-badge">4× Flash</div>
            </div>
            
            <div class="chip-info">ESP32-S3-WROOM-1-N16R8</div>
            
            <div class="specs-grid">
                <div class="spec-item">
                    <span class="spec-label">Flash Memory</span>
                    <span class="spec-value highlight">16MB</span>
                </div>
                <div class="spec-item">
                    <span class="spec-label">PSRAM</span>
                    <span class="spec-value">8MB</span>
                </div>
                <div class="spec-item">
                    <span class="spec-label">OTA Partition</span>
                    <span class="spec-value highlight">5MB per slot</span>
                </div>
                <div class="spec-item">
                    <span class="spec-label">File Storage</span>
                    <span class="spec-value highlight">6MB SPIFFS</span>
                </div>
            </div>
            
            <div class="card-footer">
                <a href="https://www.elecrow.com/esp32-display-7-0-inch-hmi-display-spi-tft-lcd-touch-screen-support-lvgl.html" 
                   target="_blank" class="product-link">View Product Page →</a>
            </div>
        </div>
    </div>
    
    <div class="hardware-help">
        <details>
            <summary>🔍 How do I identify my hardware?</summary>
            <div class="help-content">
                <p><strong>Check the back of your display for the model sticker:</strong></p>
                <ul>
                    <li><strong>Basic Model:</strong> "CrowPanel ESP32 Display 7.0-inch" or "N4R8" chip marking</li>
                    <li><strong>Advance Model:</strong> "CrowPanel ESP32 Display 7.0-inch Advance" or "N16R8" chip marking</li>
                </ul>
                <p class="warning">⚠️ <strong>Important:</strong> Installing the wrong firmware will result in a non-functional display. 
                The display pins are completely different between models.</p>
            </div>
        </details>
    </div>
</div>

<div class="install-section">
    <h2>Install Firmware</h2>
    <p id="hardware-confirm" class="hardware-confirmation">
        Selected: <strong id="selected-hardware">CrowPanel 7" Basic (4MB Flash)</strong>
    </p>
    
    <esp-web-install-button manifest="./manifest.json">
        <button slot="activate" id="install-btn">Install FluidTouch</button>
        <span slot="unsupported">
            <p style="color: #ff6b6b;">Your browser doesn't support Web Serial API.</p>
            <p>Please use Chrome, Edge, or Opera browser.</p>
        </span>
        <span slot="not-allowed">
            <p style="color: #ff6b6b;">Serial port access not allowed.</p>
            <p>Please check your browser permissions.</p>
        </span>
    </esp-web-install-button>
</div>
```

**JavaScript for Hardware Selection**:

```javascript
<script>
document.addEventListener('DOMContentLoaded', function() {
    const cards = document.querySelectorAll('.hardware-card');
    const installButton = document.querySelector('esp-web-install-button');
    const selectedText = document.getElementById('selected-hardware');
    
    // Hardware names matching manifest.json builds array
    const hardwareNames = [
        'CrowPanel 7" Basic (4MB Flash)',
        'CrowPanel 7" Advance (16MB Flash)'
    ];
    
    // Load saved selection from localStorage
    const savedBuild = localStorage.getItem('fluidtouch-hardware') || '0';
    selectHardware(savedBuild);
    
    // Add click handlers to cards
    cards.forEach(card => {
        card.addEventListener('click', function() {
            const buildIndex = this.getAttribute('data-build');
            selectHardware(buildIndex);
        });
    });
    
    function selectHardware(buildIndex) {
        // Update card selection
        cards.forEach(card => {
            card.classList.remove('selected');
            if (card.getAttribute('data-build') === buildIndex) {
                card.classList.add('selected');
            }
        });
        
        // Update confirmation text
        selectedText.textContent = hardwareNames[buildIndex];
        
        // Save selection
        localStorage.setItem('fluidtouch-hardware', buildIndex);
        
        // Update ESP Web Tools manifest selection
        // ESP Web Tools will show dropdown if multiple builds exist
        // User will see the build names from manifest.json
    }
});
</script>
```

### 4. CSS Styling

**Hardware Selection Cards**:

```css
.hardware-selection {
    background: rgba(255, 255, 255, 0.05);
    border-radius: 15px;
    padding: 30px;
    margin-bottom: 40px;
}

.hardware-selection h2 {
    color: #00d4ff;
    text-align: center;
    margin-bottom: 10px;
}

.selection-subtitle {
    text-align: center;
    color: #b3d9ff;
    margin-bottom: 30px;
}

.hardware-cards {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
    gap: 20px;
    margin-bottom: 30px;
}

.hardware-card {
    background: rgba(255, 255, 255, 0.08);
    border: 2px solid transparent;
    border-radius: 12px;
    padding: 25px;
    cursor: pointer;
    transition: all 0.3s ease;
}

.hardware-card:hover {
    background: rgba(255, 255, 255, 0.12);
    transform: translateY(-2px);
}

.hardware-card.selected {
    border-color: #00d4ff;
    background: rgba(0, 212, 255, 0.15);
    box-shadow: 0 0 20px rgba(0, 212, 255, 0.3);
}

.card-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 15px;
}

.card-header h3 {
    color: #ffffff;
    font-size: 1.3em;
    margin: 0;
}

.default-badge {
    background: #4caf50;
    color: white;
    padding: 4px 10px;
    border-radius: 12px;
    font-size: 0.75em;
    font-weight: bold;
}

.upgrade-badge {
    background: linear-gradient(135deg, #ff9800, #ff5722);
    color: white;
    padding: 4px 10px;
    border-radius: 12px;
    font-size: 0.75em;
    font-weight: bold;
}

.chip-info {
    color: #80b3ff;
    font-size: 0.9em;
    margin-bottom: 20px;
    font-family: 'Courier New', monospace;
}

.specs-grid {
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 15px;
    margin-bottom: 20px;
}

.spec-item {
    display: flex;
    flex-direction: column;
}

.spec-label {
    color: #b3d9ff;
    font-size: 0.85em;
    margin-bottom: 5px;
}

.spec-value {
    color: #ffffff;
    font-weight: bold;
    font-size: 1.1em;
}

.spec-value.highlight {
    color: #00d4ff;
}

.card-footer {
    border-top: 1px solid rgba(255, 255, 255, 0.1);
    padding-top: 15px;
    margin-top: 15px;
}

.product-link {
    color: #00d4ff;
    text-decoration: none;
    font-size: 0.9em;
    transition: color 0.3s ease;
}

.product-link:hover {
    color: #ffffff;
}

.hardware-help {
    margin-top: 20px;
}

.hardware-help details {
    background: rgba(255, 165, 0, 0.1);
    border-left: 4px solid #ffa500;
    border-radius: 8px;
    padding: 15px;
}

.hardware-help summary {
    cursor: pointer;
    color: #ffa500;
    font-weight: bold;
    user-select: none;
}

.hardware-help summary:hover {
    color: #ffb733;
}

.help-content {
    margin-top: 15px;
    color: #b3d9ff;
    line-height: 1.6;
}

.help-content .warning {
    background: rgba(255, 107, 107, 0.1);
    border-left: 4px solid #ff6b6b;
    padding: 10px;
    margin-top: 15px;
    border-radius: 4px;
}

.hardware-confirmation {
    text-align: center;
    font-size: 1.1em;
    margin-bottom: 20px;
    color: #b3d9ff;
}

.hardware-confirmation strong {
    color: #00d4ff;
}
```

### 5. GitHub Actions Workflow Updates

**Dual Build Strategy**:

```yaml
- name: Build firmware - Basic
  run: platformio run -e elecrow-crowpanel-7-basic
  
- name: Build firmware - Advance
  run: platformio run -e elecrow-crowpanel-7-advance
  
- name: Prepare firmware files
  run: |
    mkdir -p web/firmware/basic
    mkdir -p web/firmware/advance
    
    echo "=== Copying Basic firmware ==="
    cp .pio/build/elecrow-crowpanel-7-basic/firmware.bin web/firmware/basic/
    cp .pio/build/elecrow-crowpanel-7-basic/bootloader.bin web/firmware/basic/
    cp .pio/build/elecrow-crowpanel-7-basic/partitions.bin web/firmware/basic/
    
    echo "=== Copying Advance firmware ==="
    cp .pio/build/elecrow-crowpanel-7-advance/firmware.bin web/firmware/advance/
    cp .pio/build/elecrow-crowpanel-7-advance/bootloader.bin web/firmware/advance/
    cp .pio/build/elecrow-crowpanel-7-advance/partitions.bin web/firmware/advance/
    
    # boot_app0.bin is identical for both, copy to each directory
    if [ -f ~/.platformio/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin ]; then
      cp ~/.platformio/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin web/firmware/basic/
      cp ~/.platformio/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin web/firmware/advance/
    fi
    
    echo "=== Firmware files prepared ==="
    ls -la web/firmware/basic/
    ls -la web/firmware/advance/
```

**Release Notes Update**:

```yaml
- name: Create Release
  uses: softprops/action-gh-release@v1
  if: startsWith(github.ref, 'refs/tags/')
  with:
    files: |
      .pio/build/elecrow-crowpanel-7-basic/firmware.bin
      .pio/build/elecrow-crowpanel-7-advance/firmware.bin
    body: |
      ## FluidTouch ${{ steps.version.outputs.VERSION }}
      
      ### Web Installer
      Install directly from your browser: [FluidTouch Web Installer](https://jeyeager65.github.io/FluidTouch/)
      
      **Supported Hardware:**
      - CrowPanel 7" Basic (ESP32-S3 4MB Flash)
      - CrowPanel 7" Advance (ESP32-S3 16MB Flash)
      
      ### Manual Installation
      Download the appropriate firmware for your hardware:
      - **Basic Model**: `elecrow-crowpanel-7-basic.bin`
      - **Advance Model**: `elecrow-crowpanel-7-advance.bin`
      
      Flash using:
      - [ESP Web Flasher](https://esp.huhn.me/)
      - [ESPHome Flasher](https://github.com/esphome/esphome-flasher)
      - esptool.py: `esptool.py --chip esp32s3 write_flash 0x10000 firmware.bin`
      
      ### Installation Notes
      - ⚠️ **Critical**: Select the correct hardware variant or display will not function
      - First-time installation will erase existing data
      - WiFi credentials will need to be reconfigured
```

---

## User Safety & Error Prevention

### 1. Pre-Installation Warnings

**Visible Warnings**:
- Prominent message: "Installing wrong firmware will result in non-functional display"
- Hardware identification guide with expandable details
- Visual comparison of model stickers/chip markings

**Design Principle**: Make it harder to make mistakes than to do it right.

### 2. Hardware Identification Guide

**Expandable "How do I identify my hardware?" section with**:
- Photo/description of Basic model back panel
- Photo/description of Advance model back panel
- Chip marking differences (N4R8 vs N16R8)
- Model number location guide

### 3. Post-Installation Verification

**Success Indicators**:
- Display shows FluidTouch splash screen
- Touch responds to input
- WiFi configuration screen appears

**Failure Indicators** (wrong firmware):
- Blank/white screen
- Garbled/corrupted display
- No touch response
- Backlight issues

**Recovery Steps** if wrong firmware installed:
1. Reconnect USB cable
2. Return to web installer
3. Select correct hardware variant
4. Reinstall firmware

### 4. Future Enhancement: Auto-Detection

**Possible Future Feature** (v0.11.0+):
- Firmware reads chip flash size at runtime via `ESP.getFlashChipSize()`
- If mismatch detected (16MB chip but Basic firmware), show warning:
  - "⚠️ Hardware Mismatch Detected"
  - "This device has 16MB flash but is running Basic firmware (4MB)"
  - "Consider reinstalling with Advance firmware for full features"
- Non-blocking warning (firmware still works, just underutilized)

---

## Testing Strategy

### Phase 1: Pre-Advance Hardware (Current)

**Goals**: Prepare infrastructure, no functional changes.

**Tasks**:
1. Document strategy (this file)
2. Draft updated HTML/CSS/JS
3. Plan GitHub Actions changes
4. Update PlatformIO environments (Basic + Advance stub)

**Deliverable**: Documentation and draft code ready for implementation.

### Phase 2: Advance Hardware Arrival

**Goals**: Validate hardware specifications and test basic functionality.

**Tasks**:
1. Test with Elecrow example firmware first
2. Verify pin configuration matches documentation
3. Test I2C backlight control (address 0x30, 0-244 brightness levels)
4. Confirm touch controller at 0x5D (should be identical to Basic)
5. Validate 16MB flash detection

**Deliverable**: Confirmed hardware specifications, photos for identification guide.

### Phase 3: Implementation

**Goals**: Build and test multi-hardware support.

**Tasks**:
1. Implement PlatformIO multi-environment (see `hardware-porting-advance.md`)
2. Build both firmware variants successfully
3. Update web installer HTML/CSS/JS
4. Update manifest.json with dual builds
5. Update GitHub Actions workflow
6. Test local firmware flashing with both variants

**Deliverable**: Working firmware for both hardware variants, updated web installer.

### Phase 4: Integration Testing

**Goals**: End-to-end validation of complete system.

**Test Cases**:
1. **Web Installer - Basic Selection**:
   - Select Basic hardware
   - Flash to Basic device
   - Verify display, touch, WiFi, FluidNC connection
   
2. **Web Installer - Advance Selection**:
   - Select Advance hardware
   - Flash to Advance device
   - Verify display, touch, WiFi, FluidNC connection
   - Verify I2C backlight control (brightness adjustment)
   
3. **Wrong Firmware Tests** (intentional):
   - Flash Basic firmware to Advance device → Document symptoms
   - Flash Advance firmware to Basic device → Document symptoms
   - Confirm recovery by reflashing correct firmware
   
4. **Cross-Browser Testing**:
   - Chrome (primary)
   - Edge (Chromium-based)
   - Opera (Chromium-based)
   - Firefox (expected to fail - no Web Serial API)
   
5. **GitHub Actions Validation**:
   - Push to test branch
   - Verify both firmware builds succeed
   - Check artifact contains both basic/ and advance/ directories
   - Verify manifest.json updated correctly
   
6. **GitHub Pages Deployment**:
   - Tag release (test tag first)
   - Verify web installer loads
   - Verify both firmware files accessible
   - Test actual installation from deployed page

**Deliverable**: Fully tested multi-hardware web installer.

---

## Migration Path

### Version Timeline

**v0.9.1 (Current)**:
- Single hardware support (Basic only)
- Current web installer unchanged
- Tag: `v0.9.1`

**v0.10.0 (Multi-Hardware Release)**:
- Dual hardware support (Basic + Advance)
- Updated web installer with selection UI
- Updated GitHub Actions for dual builds
- Tag: `v0.10.0`
- **Requires**: Advance hardware validated

**v0.10.1+ (Enhancements)**:
- Hardware auto-detection warnings
- Enhanced identification guide with photos
- Improved error messages
- OTA update path between builds (if safe)

### Breaking Changes

**None for existing users**:
- Basic firmware remains functionally identical
- Web installer defaults to Basic (most common)
- Existing devices can update normally
- Manifest structure backward compatible (ESP Web Tools handles gracefully)

### Communication Strategy

**Release Announcement**:
- Headline: "Multi-Hardware Support: Now Compatible with CrowPanel Advance"
- Emphasize new 16MB capabilities
- Clear hardware selection guidance
- Link to identification guide

**Documentation Updates**:
- README.md: Add hardware compatibility section
- Web installer: Hardware selection UI with comparison
- GitHub releases: Separate firmware files with clear naming
- Wiki/FAQ: Hardware identification guide

---

## Benefits of Recommended Approach

### For Users:
1. **Single URL**: One web installer for all hardware (easier to share/bookmark)
2. **Clear Comparison**: Side-by-side specs help users choose correctly
3. **Visual Feedback**: Selected hardware highlighted before installation
4. **Safety**: Identification guide prevents wrong firmware installation
5. **Professional UX**: Polished interface builds confidence

### For Developers:
1. **Maintainable**: Single HTML page, single manifest.json
2. **Native Support**: ESP Web Tools built-in multi-build support
3. **Scalable**: Easy to add future hardware variants
4. **Automated**: GitHub Actions handles both builds automatically
5. **Testable**: Clear test matrix for all combinations

### For Project:
1. **Professional Image**: Shows maturity and attention to detail
2. **Reduced Support**: Clear guidance reduces user errors
3. **Future-Ready**: Architecture supports additional hardware
4. **Community Growth**: Broader hardware support = larger user base

---

## Alternative Approach (Not Recommended)

### Option 2: Separate Installer Pages

**Structure**:
- `index.html` - Landing page with hardware selection
- `install-basic.html` - Basic-specific installer
- `install-advance.html` - Advance-specific installer
- `manifest-basic.json` and `manifest-advance.json`

**Why Not Recommended**:
1. **Fragmented URLs**: Two installer links to maintain/share
2. **Duplicate Code**: Two nearly identical HTML pages
3. **Maintenance Burden**: Changes must be applied twice
4. **User Confusion**: Which link do I use?
5. **Analytics Split**: Harder to track installer usage
6. **Against Standards**: ESP Web Tools designed for multi-build manifests

**Only Use If**:
- ESP Web Tools multi-build support proves problematic (unlikely)
- Drastically different UIs needed per hardware (not the case here)
- Legal/licensing requires separate pages (not applicable)

---

## Implementation Checklist

### Prerequisites (Before Starting):
- [ ] Advance hardware received and tested
- [ ] Pin configuration validated against documentation
- [ ] I2C backlight control confirmed working
- [ ] Photos taken for identification guide

### Code Changes:
- [ ] Update `platformio.ini` with both environments
- [ ] Create custom board definition for Advance (16MB)
- [ ] Design 16MB partition table (5MB OTA + 6MB SPIFFS)
- [ ] Implement conditional compilation in `config.h`
- [ ] Update `display_driver.cpp` with hardware detection
- [ ] Add I2C backlight control for Advance
- [ ] Build both environments successfully

### Web Installer:
- [ ] Update `web/manifest.json` with dual builds
- [ ] Update `web/index.html` with hardware selection UI
- [ ] Add CSS styling for hardware cards
- [ ] Add JavaScript for card selection and localStorage
- [ ] Add hardware identification guide section
- [ ] Update requirements section with both hardware options
- [ ] Test locally with both firmware variants

### GitHub Actions:
- [ ] Update workflow to build both environments
- [ ] Update firmware copy commands for dual directories
- [ ] Update release notes template
- [ ] Attach both firmware files to releases
- [ ] Test workflow with test branch

### Documentation:
- [ ] Update main README.md with hardware compatibility
- [ ] Create hardware identification guide (with photos)
- [ ] Update web/README.md with installer instructions
- [ ] Update CHANGELOG.md with v0.10.0 notes

### Testing:
- [ ] Build Basic firmware and flash to Basic hardware
- [ ] Build Advance firmware and flash to Advance hardware
- [ ] Test wrong firmware on each (document symptoms)
- [ ] Test web installer hardware selection
- [ ] Test cross-browser compatibility
- [ ] Validate GitHub Pages deployment
- [ ] End-to-end test: tag push → auto-build → release → web install

### Release:
- [ ] Create release branch `release/v0.10.0`
- [ ] Final testing on release branch
- [ ] Update version in all files to 0.10.0
- [ ] Merge to main
- [ ] Tag `v0.10.0` and push
- [ ] Verify GitHub Actions completes successfully
- [ ] Test deployed web installer
- [ ] Announce release

---

## References

- **ESP Web Tools Documentation**: https://esphome.github.io/esp-web-tools/
- **Manifest Schema**: https://esphome.github.io/esp-web-tools/#manifest
- **Hardware Porting Guide**: `docs/hardware-porting-advance.md`
- **CrowPanel Basic Product**: https://www.elecrow.com/esp32-display-7-inch-hmi-display-rgb-tft-lcd-touch-screen-support-lvgl.html
- **CrowPanel Advance Wiki**: https://www.elecrow.com/pub/wiki/ESP32_Display-7.0_inch%28Advance_Series%29wiki.html

---

## Timeline

**Estimated Implementation**: 2-3 days after Advance hardware arrives

**Breakdown**:
- Day 1: Hardware validation, photo documentation, pin testing
- Day 2: Code implementation, PlatformIO multi-environment setup
- Day 3: Web installer updates, GitHub Actions, testing, release

**Dependencies**: Advance hardware arrival (expected this week)

---

*Document Version: 1.0*  
*Created: 2025-11-04*  
*Author: FluidTouch Development Team*
