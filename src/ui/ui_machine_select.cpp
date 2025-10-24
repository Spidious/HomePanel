#include "ui/ui_machine_select.h"
#include "ui/ui_common.h"
#include "ui/ui_tabs.h"
#include "ui/ui_theme.h"
#include "config.h"
#include <Preferences.h>

// Static member initialization
lv_obj_t *UIMachineSelect::screen = nullptr;
lv_display_t *UIMachineSelect::display = nullptr;
MachineConfig UIMachineSelect::machines[MAX_MACHINES];
lv_obj_t *UIMachineSelect::machine_buttons[MAX_MACHINES] = {nullptr};
lv_obj_t *UIMachineSelect::edit_buttons[MAX_MACHINES] = {nullptr};
lv_obj_t *UIMachineSelect::config_dialog = nullptr;
lv_obj_t *UIMachineSelect::dialog_content = nullptr;
lv_obj_t *UIMachineSelect::keyboard = nullptr;
int UIMachineSelect::editing_index = -1;
lv_obj_t *UIMachineSelect::ta_name = nullptr;
lv_obj_t *UIMachineSelect::ta_ssid = nullptr;
lv_obj_t *UIMachineSelect::ta_password = nullptr;
lv_obj_t *UIMachineSelect::ta_url = nullptr;
lv_obj_t *UIMachineSelect::ta_port = nullptr;
lv_obj_t *UIMachineSelect::dd_connection_type = nullptr;

void UIMachineSelect::show(lv_display_t *disp) {
    display = disp;
    Serial.println("UIMachineSelect: Creating machine selection screen");
    
    // Initialize default machines if none configured
    if (!MachineConfigManager::hasConfiguredMachines()) {
        MachineConfigManager::initializeDefaults();
    }
    
    // Load machines from Preferences
    MachineConfigManager::loadMachines(machines);
    
    // Create screen
    screen = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(screen, UITheme::BG_DARK, 0);
    
    // Title
    lv_obj_t *title = lv_label_create(screen);
    lv_label_set_text(title, "Select Machine");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(title, UITheme::TEXT_LIGHT, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);
    
    // Subtitle
    lv_obj_t *subtitle = lv_label_create(screen);
    lv_label_set_text(subtitle, "Choose or configure a CNC machine");
    lv_obj_set_style_text_font(subtitle, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(subtitle, UITheme::TEXT_MEDIUM, 0);
    lv_obj_align(subtitle, LV_ALIGN_TOP_MID, 0, 50);
    
    // Machine list container (760px width x 385px height with 10px padding = 740x365 usable)
    lv_obj_t *list_container = lv_obj_create(screen);
    lv_obj_set_size(list_container, 760, 385);
    lv_obj_set_style_bg_color(list_container, UITheme::BG_MEDIUM, 0);
    lv_obj_set_style_border_width(list_container, 1, 0);
    lv_obj_set_style_border_color(list_container, UITheme::BORDER_MEDIUM, 0);
    lv_obj_set_style_pad_all(list_container, 10, 0);
    lv_obj_align(list_container, LV_ALIGN_CENTER, 0, 30);
    lv_obj_clear_flag(list_container, LV_OBJ_FLAG_SCROLLABLE);
    
    refreshMachineList();
    
    // Load screen
    lv_scr_load(screen);
    
    Serial.println("UIMachineSelect: Machine selection screen displayed");
}

void UIMachineSelect::hide() {
    if (screen) {
        lv_obj_del(screen);
        screen = nullptr;
    }
}

void UIMachineSelect::refreshMachineList() {
    // Find list container
    lv_obj_t *list_container = lv_obj_get_child(screen, 2); // 3rd child (title, subtitle, container)
    
    // Clear existing buttons
    lv_obj_clean(list_container);
    
    // Create machine slots (5 total, 70px height each with 3px spacing = 365px total)
    for (int i = 0; i < MAX_MACHINES; i++) {
        int y_pos = (i * 73); // 70px button + 3px gap
        
        if (machines[i].is_configured) {
            // Machine button
            machine_buttons[i] = lv_btn_create(list_container);
            lv_obj_set_size(machine_buttons[i], 590, 70);
            lv_obj_set_pos(machine_buttons[i], 0, y_pos);
            lv_obj_set_style_bg_color(machine_buttons[i], UITheme::BG_BUTTON, 0);
            lv_obj_set_style_bg_color(machine_buttons[i], UITheme::ACCENT_PRIMARY, LV_STATE_PRESSED);
            lv_obj_add_event_cb(machine_buttons[i], onMachineSelected, LV_EVENT_CLICKED, (void*)(intptr_t)i);
            
            // Machine label with symbol
            lv_obj_t *label = lv_label_create(machine_buttons[i]);
            const char *symbol = (machines[i].connection_type == CONN_WIRELESS) ? LV_SYMBOL_WIFI : LV_SYMBOL_USB;
            String text = String(symbol) + " " + String(machines[i].name);
            lv_label_set_text(label, text.c_str());
            lv_obj_set_style_text_font(label, &lv_font_montserrat_18, 0);
            lv_obj_align(label, LV_ALIGN_LEFT_MID, 10, 0);
            
            // Edit button
            edit_buttons[i] = lv_btn_create(list_container);
            lv_obj_set_size(edit_buttons[i], 70, 70);
            lv_obj_set_pos(edit_buttons[i], 595, y_pos);
            lv_obj_set_style_bg_color(edit_buttons[i], UITheme::ACCENT_SECONDARY, 0);
            lv_obj_add_event_cb(edit_buttons[i], onEditMachine, LV_EVENT_CLICKED, (void*)(intptr_t)i);
            
            lv_obj_t *edit_label = lv_label_create(edit_buttons[i]);
            lv_label_set_text(edit_label, LV_SYMBOL_EDIT);
            lv_obj_set_style_text_font(edit_label, &lv_font_montserrat_22, 0);
            lv_obj_center(edit_label);
            
            // Delete button
            lv_obj_t *del_btn = lv_btn_create(list_container);
            lv_obj_set_size(del_btn, 70, 70);
            lv_obj_set_pos(del_btn, 670, y_pos);
            lv_obj_set_style_bg_color(del_btn, UITheme::STATE_ALARM, 0);
            lv_obj_add_event_cb(del_btn, onDeleteMachine, LV_EVENT_CLICKED, (void*)(intptr_t)i);
            
            lv_obj_t *del_label = lv_label_create(del_btn);
            lv_label_set_text(del_label, LV_SYMBOL_TRASH);
            lv_obj_set_style_text_font(del_label, &lv_font_montserrat_20, 0);
            lv_obj_center(del_label);
        } else {
            // Empty slot - show "Add Machine" button (full width within padding)
            lv_obj_t *add_btn = lv_btn_create(list_container);
            lv_obj_set_size(add_btn, 740, 70);
            lv_obj_set_pos(add_btn, 0, y_pos);
            lv_obj_set_style_bg_color(add_btn, UITheme::BG_BUTTON, 0);
            lv_obj_set_style_bg_color(add_btn, UITheme::ACCENT_SECONDARY, LV_STATE_PRESSED);
            lv_obj_set_style_border_width(add_btn, 2, 0);
            lv_obj_set_style_border_color(add_btn, UITheme::ACCENT_SECONDARY, 0);
            lv_obj_add_event_cb(add_btn, onAddMachine, LV_EVENT_CLICKED, (void*)(intptr_t)i);
            
            lv_obj_t *add_label = lv_label_create(add_btn);
            lv_label_set_text(add_label, LV_SYMBOL_PLUS " Add Machine");
            lv_obj_set_style_text_font(add_label, &lv_font_montserrat_18, 0);
            lv_obj_set_style_text_color(add_label, UITheme::ACCENT_SECONDARY, 0);
            lv_obj_center(add_label);
        }
    }
}

void UIMachineSelect::onMachineSelected(lv_event_t *e) {
    int index = (int)(intptr_t)lv_event_get_user_data(e);
    
    Serial.printf("UIMachineSelect: Machine selected: %s (index %d)\n", machines[index].name, index);
    
    // Save selection index to preferences
    MachineConfigManager::setSelectedMachineIndex(index);
    
    // Create main UI (creates and loads new screen first)
    UICommon::createMainUI();
    
    // Now safe to delete the machine selection screen
    hide();
}

void UIMachineSelect::onEditMachine(lv_event_t *e) {
    int index = (int)(intptr_t)lv_event_get_user_data(e);
    Serial.printf("UIMachineSelect: Edit machine %d\n", index);
    showConfigDialog(index);
}

void UIMachineSelect::onAddMachine(lv_event_t *e) {
    int index = (int)(intptr_t)lv_event_get_user_data(e);
    Serial.printf("UIMachineSelect: Add machine to slot %d\n", index);
    showConfigDialog(index);
}

void UIMachineSelect::onDeleteMachine(lv_event_t *e) {
    int index = (int)(intptr_t)lv_event_get_user_data(e);
    Serial.printf("UIMachineSelect: Delete machine %d\n", index);
    
    MachineConfigManager::deleteMachine(index);
    MachineConfigManager::loadMachines(machines);
    refreshMachineList();
}

void UIMachineSelect::onConfigSave(lv_event_t *e) {
    Serial.println("UIMachineSelect: Save configuration");
    
    // Read values from dialog
    const char *name = lv_textarea_get_text(ta_name);
    const char *ssid = lv_textarea_get_text(ta_ssid);
    const char *password = lv_textarea_get_text(ta_password);
    const char *url = lv_textarea_get_text(ta_url);
    const char *port_str = lv_textarea_get_text(ta_port);
    uint16_t sel = lv_dropdown_get_selected(dd_connection_type);
    
    // Validate
    if (strlen(name) == 0) {
        Serial.println("UIMachineSelect: Machine name required");
        return;
    }
    
    // Create config
    MachineConfig config;
    strncpy(config.name, name, sizeof(config.name) - 1);
    config.connection_type = (sel == 0) ? CONN_WIRELESS : CONN_WIRED;
    strncpy(config.ssid, ssid, sizeof(config.ssid) - 1);
    strncpy(config.password, password, sizeof(config.password) - 1);
    strncpy(config.fluidnc_url, url, sizeof(config.fluidnc_url) - 1);
    config.websocket_port = atoi(port_str);
    if (config.websocket_port == 0) config.websocket_port = 81;
    config.is_configured = true;
    
    // Save
    MachineConfigManager::saveMachine(editing_index, config);
    MachineConfigManager::loadMachines(machines);
    
    hideConfigDialog();
    refreshMachineList();
}

void UIMachineSelect::onConfigCancel(lv_event_t *e) {
    Serial.println("UIMachineSelect: Cancel configuration");
    hideConfigDialog();
}

void UIMachineSelect::onConnectionTypeChanged(lv_event_t *e) {
    updateConnectionFields();
}

void UIMachineSelect::updateConnectionFields() {
    uint16_t sel = lv_dropdown_get_selected(dd_connection_type);
    bool is_wireless = (sel == 0);
    
    // Enable/disable wireless-specific fields
    if (is_wireless) {
        lv_obj_clear_state(ta_ssid, LV_STATE_DISABLED);
        lv_obj_clear_state(ta_password, LV_STATE_DISABLED);
    } else {
        lv_obj_add_state(ta_ssid, LV_STATE_DISABLED);
        lv_obj_add_state(ta_password, LV_STATE_DISABLED);
    }
}

void UIMachineSelect::showConfigDialog(int index) {
    editing_index = index;
    bool is_new = !machines[index].is_configured;
    
    // Create modal background
    config_dialog = lv_obj_create(lv_scr_act());
    lv_obj_set_size(config_dialog, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(config_dialog, lv_color_make(0, 0, 0), 0);
    lv_obj_set_style_bg_opa(config_dialog, LV_OPA_70, 0);
    lv_obj_set_style_border_width(config_dialog, 0, 0);
    lv_obj_clear_flag(config_dialog, LV_OBJ_FLAG_SCROLLABLE);
    
    // Scrollable dialog content container (780x460 to fit on screen with margin)
    dialog_content = lv_obj_create(config_dialog);
    lv_obj_set_size(dialog_content, 780, 460);
    lv_obj_center(dialog_content);
    lv_obj_set_style_bg_color(dialog_content, UITheme::BG_MEDIUM, 0);
    lv_obj_set_style_border_color(dialog_content, UITheme::BORDER_MEDIUM, 0);
    lv_obj_set_style_border_width(dialog_content, 2, 0);
    lv_obj_set_flex_flow(dialog_content, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(dialog_content, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_all(dialog_content, 20, 0);
    lv_obj_set_style_pad_gap(dialog_content, 10, 0);
    
    // Title
    lv_obj_t *dlg_title = lv_label_create(dialog_content);
    lv_label_set_text(dlg_title, is_new ? "Add Machine" : "Edit Machine");
    lv_obj_set_style_text_font(dlg_title, &lv_font_montserrat_20, 0);
    lv_obj_set_width(dlg_title, LV_PCT(100));
    
    // Name field
    lv_obj_t *lbl_name = lv_label_create(dialog_content);
    lv_label_set_text(lbl_name, "Name:");
    
    ta_name = lv_textarea_create(dialog_content);
    lv_obj_set_width(ta_name, LV_PCT(100));
    lv_obj_set_height(ta_name, 40);
    lv_textarea_set_one_line(ta_name, true);
    lv_textarea_set_max_length(ta_name, 31);
    if (!is_new) lv_textarea_set_text(ta_name, machines[index].name);
    lv_obj_add_event_cb(ta_name, onTextareaFocused, LV_EVENT_FOCUSED, nullptr);
    
    // Connection Type
    lv_obj_t *lbl_type = lv_label_create(dialog_content);
    lv_label_set_text(lbl_type, "Connection:");
    
    dd_connection_type = lv_dropdown_create(dialog_content);
    lv_obj_set_width(dd_connection_type, 200);
    lv_obj_set_height(dd_connection_type, 40);
    lv_dropdown_set_options(dd_connection_type, "Wireless\nWired");
    if (!is_new) lv_dropdown_set_selected(dd_connection_type, machines[index].connection_type);
    lv_obj_add_event_cb(dd_connection_type, onConnectionTypeChanged, LV_EVENT_VALUE_CHANGED, nullptr);
    
    // Wireless credentials container (horizontal layout for SSID and Password)
    lv_obj_t *wifi_container = lv_obj_create(dialog_content);
    lv_obj_set_width(wifi_container, LV_PCT(100));
    lv_obj_set_height(wifi_container, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(wifi_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(wifi_container, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_all(wifi_container, 0, 0);
    lv_obj_set_style_border_width(wifi_container, 0, 0);
    lv_obj_set_style_bg_opa(wifi_container, LV_OPA_TRANSP, 0);
    
    // SSID column
    lv_obj_t *ssid_col = lv_obj_create(wifi_container);
    lv_obj_set_size(ssid_col, LV_PCT(48), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(ssid_col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(ssid_col, 0, 0);
    lv_obj_set_style_border_width(ssid_col, 0, 0);
    lv_obj_set_style_bg_opa(ssid_col, LV_OPA_TRANSP, 0);
    
    lv_obj_t *lbl_ssid = lv_label_create(ssid_col);
    lv_label_set_text(lbl_ssid, "WiFi SSID:");
    
    ta_ssid = lv_textarea_create(ssid_col);
    lv_obj_set_width(ta_ssid, LV_PCT(100));
    lv_obj_set_height(ta_ssid, 40);
    lv_textarea_set_one_line(ta_ssid, true);
    lv_textarea_set_max_length(ta_ssid, 32);
    if (!is_new) lv_textarea_set_text(ta_ssid, machines[index].ssid);
    lv_obj_add_event_cb(ta_ssid, onTextareaFocused, LV_EVENT_FOCUSED, nullptr);
    
    // Password column
    lv_obj_t *pwd_col = lv_obj_create(wifi_container);
    lv_obj_set_size(pwd_col, LV_PCT(48), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(pwd_col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(pwd_col, 0, 0);
    lv_obj_set_style_border_width(pwd_col, 0, 0);
    lv_obj_set_style_bg_opa(pwd_col, LV_OPA_TRANSP, 0);
    
    lv_obj_t *lbl_pwd = lv_label_create(pwd_col);
    lv_label_set_text(lbl_pwd, "Password:");
    
    ta_password = lv_textarea_create(pwd_col);
    lv_obj_set_width(ta_password, LV_PCT(100));
    lv_obj_set_height(ta_password, 40);
    lv_textarea_set_one_line(ta_password, true);
    lv_textarea_set_max_length(ta_password, 63);
    lv_textarea_set_password_mode(ta_password, true);
    if (!is_new) lv_textarea_set_text(ta_password, machines[index].password);
    lv_obj_add_event_cb(ta_password, onTextareaFocused, LV_EVENT_FOCUSED, nullptr);
    
    // FluidNC connection container (horizontal layout for URL and Port)
    lv_obj_t *fluidnc_container = lv_obj_create(dialog_content);
    lv_obj_set_width(fluidnc_container, LV_PCT(100));
    lv_obj_set_height(fluidnc_container, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(fluidnc_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(fluidnc_container, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_all(fluidnc_container, 0, 0);
    lv_obj_set_style_border_width(fluidnc_container, 0, 0);
    lv_obj_set_style_bg_opa(fluidnc_container, LV_OPA_TRANSP, 0);
    
    // URL column
    lv_obj_t *url_col = lv_obj_create(fluidnc_container);
    lv_obj_set_size(url_col, LV_PCT(65), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(url_col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(url_col, 0, 0);
    lv_obj_set_style_border_width(url_col, 0, 0);
    lv_obj_set_style_bg_opa(url_col, LV_OPA_TRANSP, 0);
    
    lv_obj_t *lbl_url = lv_label_create(url_col);
    lv_label_set_text(lbl_url, "FluidNC URL:");
    
    ta_url = lv_textarea_create(url_col);
    lv_obj_set_width(ta_url, LV_PCT(100));
    lv_obj_set_height(ta_url, 40);
    lv_textarea_set_one_line(ta_url, true);
    lv_textarea_set_max_length(ta_url, 127);
    if (!is_new) {
        lv_textarea_set_text(ta_url, machines[index].fluidnc_url);
    } else {
        lv_textarea_set_text(ta_url, "fluidnc.local");
    }
    lv_obj_add_event_cb(ta_url, onTextareaFocused, LV_EVENT_FOCUSED, nullptr);
    
    // Port column
    lv_obj_t *port_col = lv_obj_create(fluidnc_container);
    lv_obj_set_size(port_col, LV_PCT(30), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(port_col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(port_col, 0, 0);
    lv_obj_set_style_border_width(port_col, 0, 0);
    lv_obj_set_style_bg_opa(port_col, LV_OPA_TRANSP, 0);
    
    lv_obj_t *lbl_port = lv_label_create(port_col);
    lv_label_set_text(lbl_port, "Port:");
    
    ta_port = lv_textarea_create(port_col);
    lv_obj_set_width(ta_port, LV_PCT(100));
    lv_obj_set_height(ta_port, 40);
    lv_textarea_set_one_line(ta_port, true);
    lv_textarea_set_max_length(ta_port, 5);
    lv_textarea_set_accepted_chars(ta_port, "0123456789");
    if (!is_new) {
        char port_str[6];
        snprintf(port_str, sizeof(port_str), "%d", machines[index].websocket_port);
        lv_textarea_set_text(ta_port, port_str);
    } else {
        lv_textarea_set_text(ta_port, "81");
    }
    lv_obj_add_event_cb(ta_port, onTextareaFocused, LV_EVENT_FOCUSED, nullptr);
    
    // Button container
    lv_obj_t *btn_container = lv_obj_create(dialog_content);
    lv_obj_set_width(btn_container, LV_PCT(100));
    lv_obj_set_height(btn_container, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(btn_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn_container, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_all(btn_container, 0, 0);
    lv_obj_set_style_border_width(btn_container, 0, 0);
    lv_obj_set_style_bg_opa(btn_container, LV_OPA_TRANSP, 0);
    
    lv_obj_t *btn_save = lv_btn_create(btn_container);
    lv_obj_set_size(btn_save, LV_PCT(48), 50);
    lv_obj_set_style_bg_color(btn_save, UITheme::BTN_PLAY, 0);
    lv_obj_add_event_cb(btn_save, onConfigSave, LV_EVENT_CLICKED, nullptr);
    
    lv_obj_t *lbl_save = lv_label_create(btn_save);
    lv_label_set_text(lbl_save, LV_SYMBOL_OK " Save");
    lv_obj_set_style_text_font(lbl_save, &lv_font_montserrat_18, 0);
    lv_obj_center(lbl_save);
    
    lv_obj_t *btn_cancel = lv_btn_create(btn_container);
    lv_obj_set_size(btn_cancel, LV_PCT(48), 50);
    lv_obj_set_style_bg_color(btn_cancel, UITheme::BG_BUTTON, 0);
    lv_obj_add_event_cb(btn_cancel, onConfigCancel, LV_EVENT_CLICKED, nullptr);
    
    lv_obj_t *lbl_cancel = lv_label_create(btn_cancel);
    lv_label_set_text(lbl_cancel, LV_SYMBOL_CLOSE " Cancel");
    lv_obj_set_style_text_font(lbl_cancel, &lv_font_montserrat_18, 0);
    lv_obj_center(lbl_cancel);
    
    // Initialize connection fields state
    updateConnectionFields();
}

void UIMachineSelect::hideConfigDialog() {
    hideKeyboard();
    if (config_dialog) {
        lv_obj_del(config_dialog);
        config_dialog = nullptr;
        dialog_content = nullptr;
    }
    editing_index = -1;
}

void UIMachineSelect::onTextareaFocused(lv_event_t *e) {
    lv_obj_t *ta = (lv_obj_t*)lv_event_get_target(e);
    showKeyboard(ta);
}

void UIMachineSelect::showKeyboard(lv_obj_t *ta) {
    if (!keyboard) {
        // Create keyboard at screen level (not inside dialog) so it stays fixed at bottom
        keyboard = lv_keyboard_create(lv_scr_act());
        lv_obj_set_size(keyboard, SCREEN_WIDTH, 220);
        lv_obj_align(keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);
        
        // Add event handler for keyboard ready/cancel events
        lv_obj_add_event_cb(keyboard, [](lv_event_t *e) {
            UIMachineSelect::hideKeyboard();
        }, LV_EVENT_READY, nullptr);
        
        lv_obj_add_event_cb(keyboard, [](lv_event_t *e) {
            UIMachineSelect::hideKeyboard();
        }, LV_EVENT_CANCEL, nullptr);
        
        // Add click event to config_dialog background to close keyboard when clicking outside
        lv_obj_add_event_cb(config_dialog, [](lv_event_t *e) {
            UIMachineSelect::hideKeyboard();
        }, LV_EVENT_CLICKED, nullptr);
        
        // Add click event to dialog_content to close keyboard when clicking on dialog
        lv_obj_add_event_cb(dialog_content, [](lv_event_t *e) {
            lv_obj_t *target = (lv_obj_t*)lv_event_get_target(e);
            // Only close if clicking directly on dialog_content (not its children like textareas)
            if (target == dialog_content) {
                UIMachineSelect::hideKeyboard();
            }
        }, LV_EVENT_CLICKED, nullptr);
        
        // Make dialog_content scrollable and add extra padding at bottom for keyboard
        lv_obj_add_flag(dialog_content, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_pad_bottom(dialog_content, 240, 0); // Extra space for scrolling (keyboard height + margin)
    }
    
    lv_keyboard_set_textarea(keyboard, ta);
    
    // Scroll the dialog content to position the focused textarea just above keyboard
    if (dialog_content && ta) {
        // Get textarea position within dialog_content
        lv_coord_t ta_y = lv_obj_get_y(ta);
        lv_obj_t *parent = lv_obj_get_parent(ta);
        
        // Walk up parent hierarchy to get cumulative Y position
        while (parent && parent != dialog_content) {
            ta_y += lv_obj_get_y(parent);
            parent = lv_obj_get_parent(parent);
        }
        
        // Calculate scroll position to place textarea just above keyboard
        // Dialog is 460px tall, keyboard is 220px, so visible area is 240px
        // Position textarea at bottom of visible area (240px from dialog top) minus field height and margin
        lv_coord_t dialog_height = lv_obj_get_height(dialog_content);
        lv_coord_t visible_height = 240; // Height above keyboard
        lv_coord_t ta_height = lv_obj_get_height(ta);
        lv_coord_t target_position = visible_height - ta_height - 15; // 15px margin above keyboard (5px gap + 10px padding)
        
        // Scroll amount = (textarea Y position) - (where we want it)
        lv_coord_t scroll_y = ta_y - target_position;
        if (scroll_y < 0) scroll_y = 0; // Don't scroll past top
        
        lv_obj_scroll_to_y(dialog_content, scroll_y, LV_ANIM_ON);
    }
}

void UIMachineSelect::hideKeyboard() {
    if (keyboard) {
        lv_obj_del(keyboard);
        keyboard = nullptr;
        
        // Restore dialog_content to non-scrollable and remove extra padding
        if (dialog_content) {
            lv_obj_clear_flag(dialog_content, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_set_style_pad_bottom(dialog_content, 20, 0); // Back to original padding
            lv_obj_scroll_to_y(dialog_content, 0, LV_ANIM_ON); // Reset scroll position
        }
        
        // Remove click event from config_dialog (clean up event handler)
        if (config_dialog) {
            lv_obj_remove_event_cb_with_user_data(config_dialog, nullptr, nullptr);
        }
    }
}
