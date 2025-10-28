# Macros Tab Feature - Design & Implementation Notes

## Original Requirements

### Feature Overview
A Macros tab allowing users to create and execute custom GCode commands or SD card file operations, with 12 configurable macro slots per machine.

### UI Design Specifications

#### Layout
- **12 macro slots** arranged in a **4×3 grid**
- **Button size**: 185px wide × 50px tall
- **Spacing**: 195px horizontal, 60px vertical
- **Start position**: x=10, y=60 (to accommodate edit button)
- **Edit button**: 140×40px, positioned at (650, 10) in top-right corner

#### Behavior Modes

**Run Mode** (Default):
- Show only configured macros
- Unconfigured slots are hidden
- Click macro button → immediate execution
- Edit button shows "⚙ Edit" (blue BTN_CONNECT color)

**Edit Mode**:
- Show all 12 slots
- Configured slots show macro name
- Unconfigured slots show "+ Add Macro"
- Click any slot → open edit dialog
- Edit button shows "✓ Done" (green BTN_PLAY color)

#### Edit Dialog (700×400px Modal)
- **Title**: "Add Macro" or "Edit Macro" (22pt font)
- **Name field**: Text input, 64 character max, 650×40px
- **Type selection**: Radio buttons
  - "GCode Command"
  - "SD Card File"
- **Content field**: Text input, 128 character max, 650×60px
- **Buttons**:
  - "💾 Save" (green, 150×40px) - always visible
  - "🗑 Delete" (red, 150×40px) - only if macro is configured
  - "✕ Cancel" (dark gray, 150×40px) - always visible
- **Keyboard support**: Auto-popup on text field focus, 200px max height

### Data Structure Design

#### MacroType Enum
```cpp
enum MacroType {
    MACRO_GCODE = 0,    // Direct GCode command
    MACRO_SDFILE = 1    // SD card file execution
};
```

#### MacroSlot Struct
```cpp
struct MacroSlot {
    char name[65];          // Display name (64 chars + null terminator)
    MacroType type;         // GCode or SD File
    char content[128];      // Command or filename
    bool is_configured;     // Whether slot is used
};
```

#### Storage
- **Per-machine storage**: Each machine has its own set of 12 macros
- **Location**: Added to `MachineConfig` struct as `MacroSlot macros[MAX_MACROS]`
- **MAX_MACROS**: Defined as 12
- **Preferences keys**: `"m{i}_mc{j}_cfg"`, `"m{i}_mc{j}_name"`, `"m{i}_mc{j}_type"`, `"m{i}_mc{j}_cont"`
  - `i` = machine index (0-4)
  - `j` = macro slot (0-11)

### Execution Logic

#### GCode Type (MACRO_GCODE)
- Send content directly to FluidNC via WebSocket
- Example: `G0 X0 Y0 Z0` (go to origin)
- Implementation: `FluidNCClient::sendCommand(macro.content)`

#### SD File Type (MACRO_SDFILE)
- Execute SD card file via FluidNC command
- Format: `$SD/Run=filename`
- Example: Content="toolchange.nc" → Send "$SD/Run=toolchange.nc"
- Implementation: `FluidNCClient::sendCommand(("$SD/Run=" + String(macro.content)).c_str())`

### Visual Feedback
- When macro executes, button color changes to green (BTN_PLAY)
- Provides immediate visual confirmation of execution

---

## Implementation Challenges Encountered

### Critical Issue: Stack Overflow

#### Root Cause
The addition of 12 macro slots caused `MachineConfig` struct to grow significantly:

**Size Breakdown:**
- Each `MacroSlot`: ~198 bytes
  - `name[65]`: 65 bytes
  - `type`: 4 bytes (enum)
  - `content[128]`: 128 bytes
  - `is_configured`: 1 byte
- 12 macros: ~2,376 bytes
- Base `MachineConfig`: ~300 bytes
- **Total per machine**: ~2,676 bytes
- **Full array** `MachineConfig machines[MAX_MACHINES]`: **~13,380 bytes**

#### The Problem
ESP32-S3 default task stack is only **8KB (8,192 bytes)**. Multiple functions were allocating the full 5-machine array on the stack:

**Original problematic code pattern:**
```cpp
void someFunction() {
    MachineConfig machines[MAX_MACHINES];  // 13,380 bytes on stack!
    MachineConfigManager::loadMachines(machines);
    // ... use machines ...
}
```

**Functions with this issue:**
1. `MachineConfigManager::getMachine()` - loaded full array to return one machine
2. `MachineConfigManager::saveMachine()` - loaded full array to save one machine
3. `MachineConfigManager::deleteMachine()` - loaded full array to delete one machine
4. `MachineConfigManager::hasConfiguredMachines()` - loaded full array to check flags
5. `main.cpp` setup - loaded full array to find first configured machine
6. `UITabMacros::refresh()` - loaded full array to display current machine's macros
7. `UITabMacros::showEditDialog()` - loaded full array to edit one macro
8. `UITabMacros::executeMacro()` - loaded full array to execute one macro
9. `UITabMacros::saveMacro()` - loaded full array to save one macro
10. `UITabMacros::deleteMacro()` - loaded full array to delete one macro

#### Symptoms
- **Stack canary watchpoint triggered** (loopTask)
- **Guru Meditation Error** on ESP32-S3
- Boot loop - device crashes during splash screen or machine selection
- Error: `Core 1 panic'ed (Unhandled debug exception)`

### Attempted Solutions

#### Solution 1: Optimized Single-Machine Operations
Refactored functions to load/save only the needed machine directly from/to Preferences:

**Before:**
```cpp
bool getMachine(int index, MachineConfig &config) {
    MachineConfig machines[MAX_MACHINES];  // 13KB on stack
    loadMachines(machines);
    config = machines[index];
    return true;
}
```

**After:**
```cpp
bool getMachine(int index, MachineConfig &config) {
    Preferences prefs;
    prefs.begin(PREFS_NAMESPACE, true);
    String prefix = "m" + String(index) + "_";
    // Load single machine directly (only ~2.7KB needed)
    prefs.getString((prefix + "name").c_str(), config.name, sizeof(config.name));
    // ... load rest of fields ...
    prefs.end();
    return true;
}
```

**Stack savings**: ~13KB → ~2.7KB (80% reduction)

#### Solution 2: Updated All Tab Functions
Updated all `UITabMacros` methods to use `getMachine()`/`saveMachine()` instead of loading full array:
- `refresh()` - single machine load
- `showEditDialog()` - single machine load
- `executeMacro()` - single machine load
- `saveMacro()` - single machine save
- `deleteMacro()` - single machine save

#### Issue with Solution 1: Key Name Mismatch
The refactored `getMachine()` and `saveMachine()` initially used **wrong Preferences key names**, breaking WiFi connectivity:

**Incorrect keys used:**
- `"conn"` instead of `"type"` (connection type)
- `"pass"` instead of `"pwd"` (password)
- `"jxy_s"` instead of `"jxy_st"` (jog XY step)
- `"jxy_f"` instead of `"jxy_fd"` (jog XY feed)
- `"jxy_mf"` instead of `"jxy_mx"` (jog XY max feed)
- Similar issues for Z axis jog keys

This was fixed by matching the original key names from `loadMachines()`/`saveMachines()`.

### Why Changes Were Reverted
After fixing the stack overflow and key name issues, the implementation was **working correctly**. However, the user wanted to **reconsider the approach** before committing to this solution, so all changes were reverted to allow time for design review.

---

## Alternative Approaches to Consider

### Option 1: Reduce Macro Storage Size
- **Fewer macros**: 6 or 8 instead of 12
- **Shorter strings**: 
  - Name: 32 chars instead of 64
  - Content: 64 chars instead of 128
- **Impact**: ~1,188 bytes for 6 macros vs ~2,376 for 12
- **Tradeoff**: Less flexibility for users

### Option 2: Use Dynamic Allocation (Heap/PSRAM)
```cpp
MachineConfig* machines = (MachineConfig*)heap_caps_malloc(
    sizeof(MachineConfig) * MAX_MACHINES, 
    MALLOC_CAP_SPIRAM  // Allocate in PSRAM, not stack
);
// Use machines...
free(machines);
```
- **Pros**: Doesn't use stack, 8MB PSRAM available
- **Cons**: Need to manage allocation/deallocation, potential memory leaks

### Option 3: Keep Optimized Single-Machine Approach (Recommended)
The refactored solution actually works well:
- Functions only load the machine they need
- Stack usage reduced from 13KB to ~2.7KB
- No changes needed to UIMachineSelect (uses static member)
- Clear separation of concerns
- **Status**: Implemented and tested successfully before revert

### Option 4: Store Macros Separately
Create a separate storage structure for macros outside of `MachineConfig`:
```cpp
struct MacroStorage {
    MacroSlot slots[MAX_MACHINES][MAX_MACROS];
};
```
- **Pros**: Keeps MachineConfig size manageable
- **Cons**: More complex code, two storage systems to maintain

### Option 5: Increase ESP32 Task Stack Size
Modify FreeRTOS task stack size for the loop task:
- Default: 8KB
- Could increase to 16KB or 20KB
- **Pros**: Simple change, existing code pattern works
- **Cons**: Uses more RAM, may not be configurable easily in Arduino framework

---

## Implementation Checklist (When Resuming)

### Data Layer
- [ ] Add `MacroType` enum to `machine_config.h`
- [ ] Add `MacroSlot` struct to `machine_config.h`
- [ ] Add `MAX_MACROS` define (12)
- [ ] Add `macros[MAX_MACROS]` array to `MachineConfig` struct
- [ ] Update `loadMachines()` to load macro data (12 loops per machine)
- [ ] Update `saveMachines()` to save macro data (12 loops per machine)
- [ ] **CRITICAL**: Ensure stack safety - use optimized single-machine methods

### UI Layer (include/ui/tabs/ui_tab_macros.h)
- [ ] Add static members for edit mode state
- [ ] Add static pointers for UI elements (buttons, dialog, textareas)
- [ ] Add public methods: `create()`, `refresh()`, `toggleEditMode()`
- [ ] Add private methods: `showEditDialog()`, `hideEditDialog()`, `executeMacro()`, `saveMacro()`, `deleteMacro()`

### UI Implementation (src/ui/tabs/ui_tab_macros.cpp)
- [ ] Initialize static members
- [ ] Implement `create()` - 4×3 grid + edit button
- [ ] Implement `refresh()` - load single machine, update button visibility/labels
- [ ] Implement `toggleEditMode()` - switch modes, update edit button
- [ ] Implement `showEditDialog()` - 700×400 modal with name/type/content fields
- [ ] Implement `hideEditDialog()` - cleanup
- [ ] Implement `executeMacro()` - handle GCode and SD file types
- [ ] Implement `saveMacro()` - validate, save single machine
- [ ] Implement `deleteMacro()` - clear macro, save single machine
- [ ] Add event handlers for all buttons and textareas
- [ ] Add keyboard support for text inputs

### Testing
- [ ] Verify no stack overflow during boot
- [ ] Test macro creation (GCode type)
- [ ] Test macro creation (SD File type)
- [ ] Test macro editing
- [ ] Test macro deletion
- [ ] Test macro execution (GCode)
- [ ] Test macro execution (SD file)
- [ ] Verify macros are per-machine (switch machines, check different macros)
- [ ] Test edit mode toggle
- [ ] Test run mode (unconfigured slots hidden)
- [ ] Test with all 12 slots configured
- [ ] Test WiFi connectivity still works (verify key names correct)
- [ ] Monitor heap/PSRAM usage during operation

---

## Key Lessons Learned

1. **ESP32 stack is limited (8KB)** - Always consider struct sizes when adding arrays
2. **Preferences key names matter** - Must match exactly between load and save operations
3. **Single-machine operations are safer** - Load only what you need to avoid stack issues
4. **Test incrementally** - Stack overflows can cause cryptic boot loops
5. **Memory architecture matters** - PSRAM available (8MB) but stack is separate and limited

---

## Status
**Reverted** - All changes backed out to allow design reconsideration. The implementation was functional but user wants to review approach before committing.

**Files affected** (now restored to pre-macro state):
- `include/ui/machine_config.h`
- `include/ui/tabs/ui_tab_macros.h`
- `src/ui/machine_config.cpp`
- `src/ui/tabs/ui_tab_macros.cpp`
- `src/main.cpp`

**Recommended next step**: Option 3 (Keep optimized single-machine approach) - it was working correctly when reverted.
