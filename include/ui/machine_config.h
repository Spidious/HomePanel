#ifndef MACHINE_CONFIG_H
#define MACHINE_CONFIG_H

#include <Arduino.h>

#define MAX_MACHINES 5

enum ConnectionType {
    CONN_WIRED = 0,
    CONN_WIRELESS = 1
};

struct MachineConfig {
    char name[32];
    ConnectionType connection_type;
    char ssid[33];           // WiFi SSID (max 32 chars + null)
    char password[64];       // WiFi password (max 63 chars + null)
    char fluidnc_url[128];   // FluidNC URL (e.g., "192.168.1.100" or "fluidnc.local")
    uint16_t websocket_port; // WebSocket port (default 81)
    bool is_configured;      // Whether this slot has a valid machine
    
    // Constructor with defaults
    MachineConfig() : connection_type(CONN_WIRELESS), websocket_port(81), is_configured(false) {
        name[0] = '\0';
        ssid[0] = '\0';
        password[0] = '\0';
        fluidnc_url[0] = '\0';
    }
};

class MachineConfigManager {
public:
    // Load all machines from Preferences
    static void loadMachines(MachineConfig machines[MAX_MACHINES]);
    
    // Save all machines to Preferences
    static void saveMachines(const MachineConfig machines[MAX_MACHINES]);
    
    // Get a specific machine by index
    static bool getMachine(int index, MachineConfig &config);
    
    // Save a specific machine by index
    static bool saveMachine(int index, const MachineConfig &config);
    
    // Delete a machine (mark as unconfigured)
    static bool deleteMachine(int index);
    
    // Get the currently selected machine index
    static int getSelectedMachineIndex();
    
    // Set the currently selected machine index
    static void setSelectedMachineIndex(int index);
    
    // Get currently selected machine config
    static bool getSelectedMachine(MachineConfig &config);
    
    // Initialize with default machines (for first run)
    static void initializeDefaults();
    
    // Check if machines are configured
    static bool hasConfiguredMachines();
};

#endif // MACHINE_CONFIG_H
