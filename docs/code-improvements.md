# FluidTouch Code Improvements & Refactoring

*Last Updated: November 1, 2025*

This document outlines potential code improvements, refactoring opportunities, and architectural enhancements identified during pre-release code review. Items are prioritized for implementation.

---

## High Priority (Before Release)

### 1. Remove Legacy TODO Comments
**Location:** `src/ui/tabs/control/ui_tab_control_joystick.cpp` (lines 179, 308)
```cpp
// TODO: Legacy comment - command sending now implemented above
```
**Action:** Remove obsolete comments that reference completed work.

### 2. Add Version Display
**Current State:** Version defined in `config.h` but not displayed anywhere
```cpp
#define FLUIDTOUCH_VERSION "v0.1-ALPHA"
```
**Action:** Add version display to:
- Splash screen (bottom corner)
- About dialog in Settings tab
- Or dedicated info section

**Enhanced Version Management:**
```cpp
// Recommended addition to config.h:
#define FLUIDTOUCH_VERSION_MAJOR 0
#define FLUIDTOUCH_VERSION_MINOR 1
#define FLUIDTOUCH_VERSION_PATCH 0
#define FLUIDTOUCH_VERSION_STRING "v0.1.0-alpha"
#define FLUIDTOUCH_BUILD_DATE __DATE__
#define FLUIDTOUCH_BUILD_TIME __TIME__
```

### 3. Consolidate Magic Numbers
**Problem:** Hardcoded values scattered across codebase

**Examples Found:**
```cpp
// ui_tab_files.cpp:
static char filename_storage[10][128];  // Should be constants

// ui_tab_macros.cpp:
#define MAX_MACROS 10  // Good location, but inconsistent with other limits
```

**Recommended Addition to `config.h`:**
```cpp
// Dialog and UI Limits
#define MAX_CONCURRENT_DIALOGS 10
#define MAX_FILENAME_LENGTH 128
#define MAX_PATH_LENGTH 256
#define MAX_MACHINES 5              // Already in machine_config.h - consider moving
#define MAX_MACROS 10               // Already in ui_tab_macros.cpp - consider moving
#define MAX_FILES_DISPLAYED 100     // Currently hardcoded in ui_tab_files.cpp
```

### 4. Screenshot Server Security Review
**Current State:**
```cpp
// config.h:
#define ENABLE_SCREENSHOT_SERVER true
```

**Security Concerns:**
- No authentication on screenshot endpoint
- Enabled by default in all builds
- Could expose machine workspace layout

**Recommendations:**
1. Add build-time flag:
   ```cpp
   #ifdef DEBUG_BUILD
       #define ENABLE_SCREENSHOT_SERVER true
   #else
       #define ENABLE_SCREENSHOT_SERVER false
   #endif
   ```

2. Add simple token authentication:
   ```cpp
   // Generate random token on boot
   String screenshot_token = String(random(0xFFFFFFFF), HEX);
   // Access via: http://IP/screenshot?token=XXXXXXXX
   ```

3. Or disable completely for production, only enable via serial command

---

## Medium Priority (Nice to Have)

### 5. Extract Common Delete Dialog Code
**Problem:** Nearly identical delete confirmation dialogs in two files

**Current Duplication:**
- `src/ui/tabs/ui_tab_files.cpp` - File delete dialog (~80 lines)
- `src/ui/tabs/ui_tab_macros.cpp` - Macro delete dialog (~80 lines)

**Proposed Solution:**
Create shared dialog utility:

```cpp
// include/ui/ui_dialogs.h
#ifndef UI_DIALOGS_H
#define UI_DIALOGS_H

#include <lvgl.h>
#include <functional>

namespace UIDialogs {
    /**
     * Show a standardized delete confirmation dialog
     * @param item_name Name of item to delete (displayed centered, no quotes)
     * @param item_type Type descriptor ("File", "Macro", etc.)
     * @param on_confirm Callback executed when user confirms deletion
     * @param on_cancel Optional callback for cancellation (default: just close)
     */
    void showDeleteConfirmation(
        const char* item_name,
        const char* item_type,
        std::function<void()> on_confirm,
        std::function<void()> on_cancel = nullptr
    );
    
    /**
     * Generic modal message box with custom buttons
     */
    void showMessageBox(
        const char* title,
        const char* message,
        const char* btn1_text,
        const char* btn2_text = nullptr,
        std::function<void()> btn1_callback = nullptr,
        std::function<void()> btn2_callback = nullptr
    );
}

#endif
```

**Benefits:**
- Single source of truth for dialog styling
- Consistent UX across all confirmations
- Easier to update styling globally
- ~150 lines of code reduction

### 6. Logging System with Debug Levels
**Problem:** `Serial.printf()` scattered throughout, no control over verbosity

**Current State:**
```cpp
Serial.printf("[Files] Loading...\n");
Serial.printf("[Probe] X- probe result: %.3f\n", result);
```

**Proposed Solution:**
```cpp
// In config.h:
#define DEBUG_LEVEL 2  // 0=none, 1=errors, 2=info, 3=verbose

// In new include/debug.h:
#if DEBUG_LEVEL >= 1
    #define LOG_ERROR(tag, fmt, ...)   Serial.printf("[ERROR][%s] " fmt "\n", tag, ##__VA_ARGS__)
#else
    #define LOG_ERROR(tag, fmt, ...)   ((void)0)
#endif

#if DEBUG_LEVEL >= 2
    #define LOG_INFO(tag, fmt, ...)    Serial.printf("[INFO][%s] " fmt "\n", tag, ##__VA_ARGS__)
#else
    #define LOG_INFO(tag, fmt, ...)    ((void)0)
#endif

#if DEBUG_LEVEL >= 3
    #define LOG_VERBOSE(tag, fmt, ...) Serial.printf("[DEBUG][%s] " fmt "\n", tag, ##__VA_ARGS__)
#else
    #define LOG_VERBOSE(tag, fmt, ...) ((void)0)
#endif

// Usage:
LOG_INFO("Files", "Loading file list for: %s", path.c_str());
LOG_VERBOSE("Probe", "X- probe result: %.3f", result);
LOG_ERROR("FluidNC", "Connection failed: %s", error);
```

**Benefits:**
- Control debug output verbosity at compile time
- Zero overhead when disabled
- Consistent log format
- Tagged output for filtering

### 7. Null Pointer Check Consistency
**Problem:** Some code checks pointers before use, some doesn't

**Examples:**
```cpp
// Good:
if (status_label) {
    lv_label_set_text(status_label, "Ready");
}

// Missing check:
lv_label_set_text(some_label, text);  // Could crash if some_label is nullptr
```

**Action:** 
- Audit all LVGL object usage
- Add defensive checks or ensure initialization guarantees
- Consider helper macros:
  ```cpp
  #define SAFE_SET_TEXT(obj, text) if(obj) lv_label_set_text(obj, text)
  ```

### 8. Document or Remove Unused Font
**Files Found:**
- `include/jetbrains_mono_16.h`
- `src/jetbrains_mono_16.c`

**Action:**
1. Search codebase for usage
2. If unused: Remove files and references from build
3. If intended for future use: Document purpose in README or comments
4. If used but not obvious: Add comment explaining where it's used

---

## Low Priority (Post-Release)

### 9. File Structure Reorganization
**Current Structure:**
```
include/
  ├── config.h
  ├── display_driver.h
  ├── fluidnc_client.h
  ├── fluidnc_logo.h
  ├── jetbrains_mono_16.h
  ├── lv_conf.h
  ├── screenshot_server.h
  ├── touch_driver.h
  └── ui/
      ├── machine_config.h
      ├── ui_*.h
      └── tabs/
```

**Proposed Structure:**
```
include/
  ├── config.h              # Global configuration
  ├── version.h             # Version management
  ├── debug.h               # Logging macros
  ├── core/                 # Core system
  │   ├── display_driver.h
  │   └── touch_driver.h
  ├── hardware/             # Hardware-specific
  │   ├── fluidnc_logo.h
  │   └── screenshot_server.h
  ├── network/              # Network/Communication
  │   └── fluidnc_client.h
  ├── ui/                   # All UI (existing structure good)
  │   ├── ui_constants.h    # NEW: UI-related constants
  │   ├── ui_dialogs.h      # NEW: Shared dialog utilities
  │   ├── machine_config.h
  │   └── ...
  └── lvgl/                 # LVGL configuration
      └── lv_conf.h
```

**Benefits:**
- Clearer separation of concerns
- Easier to navigate for new contributors
- Logical grouping of related functionality

**Caution:** Breaking change - update all `#include` paths

### 10. Static Pointer Refactoring
**Problem:** Many files use file-scope static pointers for state management

**Examples:**
```cpp
// ui_tab_control_probe.cpp:
static lv_obj_t* feed_input_ptr = nullptr;
static lv_obj_t* dist_input_ptr = nullptr;
static lv_obj_t* retract_input_ptr = nullptr;
static lv_obj_t* thickness_input_ptr = nullptr;

// screenshot_server.cpp:
static DisplayDriver* display_driver_instance = nullptr;
static uint16_t* screenshot_buffer = nullptr;
```

**Considerations:**
- Current approach works fine for single-threaded embedded
- Moving to class static members adds encapsulation
- Singleton pattern adds complexity without much benefit here
- Only refactor if clear benefit emerges

**Recommendation:** Document rationale for current approach rather than refactor

### 11. Constants Deduplication
**Problem:** Some constant arrays appear in multiple places

**Example - Step values defined separately for XY and Z:**
```cpp
// ui_tab_control_jog.cpp:
static const float xy_step_values[] = {0.1f, 1.0f, 10.0f, 50.0f, 100.0f, 500.0f};
static const float z_step_values[] = {0.1f, 1.0f, 10.0f};
```

**Proposed Consolidation:**
```cpp
// In include/ui/ui_constants.h:
namespace UIConstants {
    namespace Jog {
        constexpr float STEP_VALUES_XY[] = {0.1f, 1.0f, 10.0f, 50.0f, 100.0f, 500.0f};
        constexpr float STEP_VALUES_Z[] = {0.1f, 1.0f, 10.0f};
        constexpr size_t STEP_VALUES_XY_COUNT = sizeof(STEP_VALUES_XY) / sizeof(STEP_VALUES_XY[0]);
        constexpr size_t STEP_VALUES_Z_COUNT = sizeof(STEP_VALUES_Z) / sizeof(STEP_VALUES_Z[0]);
    }
}
```

### 12. Memory Allocation Documentation
**Current Mix:**
- `std::vector` for dynamic lists (file_list_with_sizes)
- Fixed arrays for UI pointers
- Static storage arrays for callback data
- `heap_caps_malloc()` for large buffers (PSRAM)

**Action:** Create `docs/memory-strategy.md` explaining:
- When to use each approach
- PSRAM vs regular heap guidelines
- Why certain patterns were chosen
- Memory usage expectations

---

## Notes & Considerations

### Screenshot Server Production Use
Decision needed:
- Keep enabled for remote debugging in field?
- Disable by default, enable via serial command?
- Add authentication and keep enabled?
- Remove completely before release?

### Breaking Changes
Items marked as "breaking changes" should be done between major versions:
- File structure reorganization (#9)
- API changes to existing modules
- Configuration format changes

### Testing After Changes
Any code changes should include:
- Build test (verify compilation)
- Flash and boot test (verify no runtime errors)
- Feature test (verify affected features work)
- Memory test (check heap/PSRAM usage)

### Future Enhancements Not Listed
These improvements focus on code quality and architecture. Feature enhancements (new tabs, new functionality) should be tracked separately in a features/roadmap document.

---

## Implementation Checklist

**Quick Wins (Can do now):**
- [ ] Remove TODO comments in joystick file
- [ ] Add version display to splash screen
- [ ] Move magic numbers to config.h
- [ ] Review screenshot server settings
- [ ] Remove or document jetbrains_mono_16 font

**Requires More Work:**
- [ ] Create shared dialog utility (ui_dialogs.h/cpp)
- [ ] Implement debug logging system
- [ ] Audit and add null pointer checks
- [ ] Consolidate duplicate constants

**Post-Release:**
- [ ] Consider file structure reorganization
- [ ] Document memory allocation strategies
- [ ] Evaluate static pointer patterns

---

*Generated from pre-release code review - November 2025*
