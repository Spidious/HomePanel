#include "ui/machine_config.h"
#include "config.h"
#include <Preferences.h>
#include <Arduino.h>

void MachineConfigManager::loadMachines(MachineConfig machines[MAX_MACHINES]) {
    Preferences prefs;
    prefs.begin(PREFS_NAMESPACE, true); // Read-only
    
    for (int i = 0; i < MAX_MACHINES; i++) {
        String prefix = "m" + String(i) + "_";
        
        machines[i].is_configured = prefs.getBool((prefix + "cfg").c_str(), false);
        
        if (machines[i].is_configured) {
            prefs.getString((prefix + "name").c_str(), machines[i].name, sizeof(machines[i].name));
            machines[i].connection_type = (ConnectionType)prefs.getUChar((prefix + "type").c_str(), CONN_WIRELESS);
            prefs.getString((prefix + "ssid").c_str(), machines[i].ssid, sizeof(machines[i].ssid));
            prefs.getString((prefix + "pwd").c_str(), machines[i].password, sizeof(machines[i].password));
            prefs.getString((prefix + "url").c_str(), machines[i].fluidnc_url, sizeof(machines[i].fluidnc_url));
            machines[i].websocket_port = prefs.getUShort((prefix + "port").c_str(), 81);
        }
    }
    
    prefs.end();
}

void MachineConfigManager::saveMachines(const MachineConfig machines[MAX_MACHINES]) {
    Preferences prefs;
    prefs.begin(PREFS_NAMESPACE, false); // Read-write
    
    for (int i = 0; i < MAX_MACHINES; i++) {
        String prefix = "m" + String(i) + "_";
        
        prefs.putBool((prefix + "cfg").c_str(), machines[i].is_configured);
        
        if (machines[i].is_configured) {
            prefs.putString((prefix + "name").c_str(), machines[i].name);
            prefs.putUChar((prefix + "type").c_str(), (uint8_t)machines[i].connection_type);
            prefs.putString((prefix + "ssid").c_str(), machines[i].ssid);
            prefs.putString((prefix + "pwd").c_str(), machines[i].password);
            prefs.putString((prefix + "url").c_str(), machines[i].fluidnc_url);
            prefs.putUShort((prefix + "port").c_str(), machines[i].websocket_port);
        }
    }
    
    prefs.end();
}

bool MachineConfigManager::getMachine(int index, MachineConfig &config) {
    if (index < 0 || index >= MAX_MACHINES) return false;
    
    MachineConfig machines[MAX_MACHINES];
    loadMachines(machines);
    
    if (machines[index].is_configured) {
        config = machines[index];
        return true;
    }
    return false;
}

bool MachineConfigManager::saveMachine(int index, const MachineConfig &config) {
    if (index < 0 || index >= MAX_MACHINES) return false;
    
    MachineConfig machines[MAX_MACHINES];
    loadMachines(machines);
    
    machines[index] = config;
    machines[index].is_configured = true;
    
    saveMachines(machines);
    return true;
}

bool MachineConfigManager::deleteMachine(int index) {
    if (index < 0 || index >= MAX_MACHINES) return false;
    
    MachineConfig machines[MAX_MACHINES];
    loadMachines(machines);
    
    machines[index] = MachineConfig(); // Reset to defaults
    machines[index].is_configured = false;
    
    saveMachines(machines);
    return true;
}

int MachineConfigManager::getSelectedMachineIndex() {
    Preferences prefs;
    prefs.begin(PREFS_NAMESPACE, true);
    int index = prefs.getInt("sel_machine", -1);
    prefs.end();
    return index;
}

void MachineConfigManager::setSelectedMachineIndex(int index) {
    Preferences prefs;
    prefs.begin(PREFS_NAMESPACE, false);
    prefs.putInt("sel_machine", index);
    prefs.end();
}

bool MachineConfigManager::getSelectedMachine(MachineConfig &config) {
    int index = getSelectedMachineIndex();
    if (index >= 0 && index < MAX_MACHINES) {
        return getMachine(index, config);
    }
    return false;
}

void MachineConfigManager::initializeDefaults() {
    MachineConfig machines[MAX_MACHINES];
    
    // Machine 0: V1E LowRider 3
    strncpy(machines[0].name, "V1E LowRider 3", sizeof(machines[0].name));
    machines[0].connection_type = CONN_WIRELESS;
    strncpy(machines[0].ssid, "", sizeof(machines[0].ssid));  // User must configure
    strncpy(machines[0].password, "", sizeof(machines[0].password));
    strncpy(machines[0].fluidnc_url, "fluidnc.local", sizeof(machines[0].fluidnc_url));
    machines[0].websocket_port = 81;
    machines[0].is_configured = true;
    
    // Machine 1: Pen Plotter
    strncpy(machines[1].name, "Pen Plotter", sizeof(machines[1].name));
    machines[1].connection_type = CONN_WIRELESS;
    strncpy(machines[1].ssid, "", sizeof(machines[1].ssid));
    strncpy(machines[1].password, "", sizeof(machines[1].password));
    strncpy(machines[1].fluidnc_url, "fluidnc.local", sizeof(machines[1].fluidnc_url));
    machines[1].websocket_port = 81;
    machines[1].is_configured = true;
    
    // Machine 2: Yeagbot
    strncpy(machines[2].name, "Yeagbot", sizeof(machines[2].name));
    machines[2].connection_type = CONN_WIRELESS;
    strncpy(machines[2].ssid, "", sizeof(machines[2].ssid));
    strncpy(machines[2].password, "", sizeof(machines[2].password));
    strncpy(machines[2].fluidnc_url, "fluidnc.local", sizeof(machines[2].fluidnc_url));
    machines[2].websocket_port = 81;
    machines[2].is_configured = true;
    
    // Machine 3: Test Wired Machine
    strncpy(machines[3].name, "Test Wired Machine", sizeof(machines[3].name));
    machines[3].connection_type = CONN_WIRED;
    machines[3].websocket_port = 81;
    machines[3].is_configured = true;
    
    // Machine 4: Empty slot
    machines[4].is_configured = false;
    
    saveMachines(machines);
    Serial.println("MachineConfigManager: Initialized default machines");
}

bool MachineConfigManager::hasConfiguredMachines() {
    MachineConfig machines[MAX_MACHINES];
    loadMachines(machines);
    
    for (int i = 0; i < MAX_MACHINES; i++) {
        if (machines[i].is_configured) {
            return true;
        }
    }
    return false;
}
