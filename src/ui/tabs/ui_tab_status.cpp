#include "ui/tabs/ui_tab_status.h"
#include "ui/ui_theme.h"
#include <cstring>
#include <stdio.h>

// Static member initialization
lv_obj_t *UITabStatus::lbl_message = nullptr;
lv_obj_t *UITabStatus::lbl_state = nullptr;
lv_obj_t *UITabStatus::lbl_wpos_x = nullptr;
lv_obj_t *UITabStatus::lbl_wpos_y = nullptr;
lv_obj_t *UITabStatus::lbl_wpos_z = nullptr;
lv_obj_t *UITabStatus::lbl_mpos_x = nullptr;
lv_obj_t *UITabStatus::lbl_mpos_y = nullptr;
lv_obj_t *UITabStatus::lbl_mpos_z = nullptr;
lv_obj_t *UITabStatus::lbl_feed_value = nullptr;
lv_obj_t *UITabStatus::lbl_feed_override = nullptr;
lv_obj_t *UITabStatus::lbl_spindle_value = nullptr;
lv_obj_t *UITabStatus::lbl_spindle_override = nullptr;
lv_obj_t *UITabStatus::lbl_modal_wcs = nullptr;
lv_obj_t *UITabStatus::lbl_modal_plane = nullptr;
lv_obj_t *UITabStatus::lbl_modal_dist = nullptr;
lv_obj_t *UITabStatus::lbl_modal_units = nullptr;
lv_obj_t *UITabStatus::lbl_modal_motion = nullptr;
lv_obj_t *UITabStatus::lbl_modal_spindle = nullptr;
lv_obj_t *UITabStatus::lbl_modal_coolant = nullptr;
lv_obj_t *UITabStatus::lbl_modal_tool = nullptr;

// Cached values initialization (for delta checking)
float UITabStatus::last_wpos_x = -9999.0f;
float UITabStatus::last_wpos_y = -9999.0f;
float UITabStatus::last_wpos_z = -9999.0f;
float UITabStatus::last_mpos_x = -9999.0f;
float UITabStatus::last_mpos_y = -9999.0f;
float UITabStatus::last_mpos_z = -9999.0f;
float UITabStatus::last_feed_rate = -1.0f;
float UITabStatus::last_feed_override = -1.0f;
float UITabStatus::last_spindle_speed = -1.0f;
float UITabStatus::last_spindle_override = -1.0f;

void UITabStatus::create(lv_obj_t *tab) {
    // Industrial Electronics Style Status Display
    lv_obj_set_style_bg_color(tab, UITheme::BG_BLACK, LV_PART_MAIN);
    
    // MACHINE STATE - Industrial header
    lv_obj_t *state_label = lv_label_create(tab);
    lv_label_set_text(state_label, "STATE");
    lv_obj_set_style_text_font(state_label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(state_label, UITheme::TEXT_DISABLED, 0);
    lv_obj_set_pos(state_label, 15, 5);

    lbl_state = lv_label_create(tab);
    lv_label_set_text(lbl_state, "IDLE");
    lv_obj_set_style_text_font(lbl_state, &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_color(lbl_state, UITheme::STATE_IDLE, 0);
    lv_obj_set_pos(lbl_state, 15, 25);

    // MESSAGE - Right side of State section
    lv_obj_t *message_label = lv_label_create(tab);
    lv_label_set_text(message_label, "MESSAGE");
    lv_obj_set_style_text_font(message_label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(message_label, UITheme::TEXT_DISABLED, 0);
    lv_obj_set_pos(message_label, 250, 5);

    lbl_message = lv_label_create(tab);
    lv_label_set_text(lbl_message, "No messages");
    lv_obj_set_style_text_font(lbl_message, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(lbl_message, UITheme::UI_INFO, 0);
    lv_obj_set_size(lbl_message, 520, 40);  // Set width to wrap text
    lv_label_set_long_mode(lbl_message, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(lbl_message, 250, 25);

    // Separator line
    lv_obj_t *line1 = lv_obj_create(tab);
    lv_obj_set_size(line1, 770, 2);
    lv_obj_set_pos(line1, 15, 70);
    lv_obj_set_style_bg_color(line1, UITheme::BG_BUTTON, 0);
    lv_obj_set_style_border_width(line1, 0, 0);

    // WORK POSITION - Left column
    lv_obj_t *wpos_header = lv_label_create(tab);
    lv_label_set_text(wpos_header, "WORK POSITION");
    lv_obj_set_style_text_font(wpos_header, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(wpos_header, UITheme::TEXT_DISABLED, 0);
    lv_obj_set_pos(wpos_header, 15, 80);

    lbl_wpos_x = lv_label_create(tab);
    lv_label_set_text(lbl_wpos_x, "X  0000.000");
    lv_obj_set_style_text_font(lbl_wpos_x, &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_color(lbl_wpos_x, UITheme::AXIS_X, 0);
    lv_obj_set_pos(lbl_wpos_x, 15, 105);

    lbl_wpos_y = lv_label_create(tab);
    lv_label_set_text(lbl_wpos_y, "Y  0000.000");
    lv_obj_set_style_text_font(lbl_wpos_y, &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_color(lbl_wpos_y, UITheme::AXIS_Y, 0);
    lv_obj_set_pos(lbl_wpos_y, 15, 150);

    lbl_wpos_z = lv_label_create(tab);
    lv_label_set_text(lbl_wpos_z, "Z  0000.000");
    lv_obj_set_style_text_font(lbl_wpos_z, &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_color(lbl_wpos_z, UITheme::AXIS_Z, 0);
    lv_obj_set_pos(lbl_wpos_z, 15, 195);

    // MACHINE POSITION - Center column (moved right for wider values)
    lv_obj_t *mpos_header = lv_label_create(tab);
    lv_label_set_text(mpos_header, "MACHINE POSITION");
    lv_obj_set_style_text_font(mpos_header, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(mpos_header, UITheme::TEXT_DISABLED, 0);
    lv_obj_set_pos(mpos_header, 250, 80);

    lbl_mpos_x = lv_label_create(tab);
    lv_label_set_text(lbl_mpos_x, "X  0000.000");
    lv_obj_set_style_text_font(lbl_mpos_x, &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_color(lbl_mpos_x, UITheme::AXIS_X, 0);
    lv_obj_set_pos(lbl_mpos_x, 250, 105);

    lbl_mpos_y = lv_label_create(tab);
    lv_label_set_text(lbl_mpos_y, "Y  0000.000");
    lv_obj_set_style_text_font(lbl_mpos_y, &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_color(lbl_mpos_y, UITheme::AXIS_Y, 0);
    lv_obj_set_pos(lbl_mpos_y, 250, 150);

    lbl_mpos_z = lv_label_create(tab);
    lv_label_set_text(lbl_mpos_z, "Z  0000.000");
    lv_obj_set_style_text_font(lbl_mpos_z, &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_color(lbl_mpos_z, UITheme::AXIS_Z, 0);
    lv_obj_set_pos(lbl_mpos_z, 250, 195);

    // MODAL STATES - Right column (labels at x=500, values at x=620)
    lv_obj_t *modal_header = lv_label_create(tab);
    lv_label_set_text(modal_header, "MODAL");
    lv_obj_set_style_text_font(modal_header, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(modal_header, UITheme::TEXT_DISABLED, 0);
    lv_obj_set_pos(modal_header, 500, 80);

    // WCS
    lv_obj_t *status_coord_label = lv_label_create(tab);
    lv_label_set_text(status_coord_label, "WCS");
    lv_obj_set_style_text_font(status_coord_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(status_coord_label, UITheme::ACCENT_SECONDARY, 0);
    lv_obj_set_pos(status_coord_label, 500, 105);
    
    lbl_modal_wcs = lv_label_create(tab);
    lv_label_set_text(lbl_modal_wcs, "G54");
    lv_obj_set_style_text_font(lbl_modal_wcs, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(lbl_modal_wcs, UITheme::POS_MODAL, 0);
    lv_obj_set_pos(lbl_modal_wcs, 620, 105);

    // PLANE
    lv_obj_t *status_plane_label = lv_label_create(tab);
    lv_label_set_text(status_plane_label, "PLANE");
    lv_obj_set_style_text_font(status_plane_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(status_plane_label, UITheme::ACCENT_SECONDARY, 0);
    lv_obj_set_pos(status_plane_label, 500, 134);
    
    lbl_modal_plane = lv_label_create(tab);
    lv_label_set_text(lbl_modal_plane, "G17");
    lv_obj_set_style_text_font(lbl_modal_plane, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(lbl_modal_plane, UITheme::UI_SECONDARY, 0);
    lv_obj_set_pos(lbl_modal_plane, 620, 134);

    // DIST
    lv_obj_t *status_dist_label = lv_label_create(tab);
    lv_label_set_text(status_dist_label, "DIST");
    lv_obj_set_style_text_font(status_dist_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(status_dist_label, UITheme::ACCENT_SECONDARY, 0);
    lv_obj_set_pos(status_dist_label, 500, 163);
    
    lbl_modal_dist = lv_label_create(tab);
    lv_label_set_text(lbl_modal_dist, "G90");
    lv_obj_set_style_text_font(lbl_modal_dist, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(lbl_modal_dist, UITheme::UI_SECONDARY, 0);
    lv_obj_set_pos(lbl_modal_dist, 620, 163);

    // UNITS
    lv_obj_t *status_units_label = lv_label_create(tab);
    lv_label_set_text(status_units_label, "UNITS");
    lv_obj_set_style_text_font(status_units_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(status_units_label, UITheme::ACCENT_SECONDARY, 0);
    lv_obj_set_pos(status_units_label, 500, 192);
    
    lbl_modal_units = lv_label_create(tab);
    lv_label_set_text(lbl_modal_units, "G21");
    lv_obj_set_style_text_font(lbl_modal_units, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(lbl_modal_units, UITheme::POS_WORK, 0);
    lv_obj_set_pos(lbl_modal_units, 620, 192);

    // MOTION
    lv_obj_t *status_motion_label = lv_label_create(tab);
    lv_label_set_text(status_motion_label, "MOTION");
    lv_obj_set_style_text_font(status_motion_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(status_motion_label, UITheme::ACCENT_SECONDARY, 0);
    lv_obj_set_pos(status_motion_label, 500, 221);
    
    lbl_modal_motion = lv_label_create(tab);
    lv_label_set_text(lbl_modal_motion, "G0");
    lv_obj_set_style_text_font(lbl_modal_motion, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(lbl_modal_motion, UITheme::UI_INFO, 0);
    lv_obj_set_pos(lbl_modal_motion, 620, 221);

    // SPINDLE
    lv_obj_t *status_spindle_label = lv_label_create(tab);
    lv_label_set_text(status_spindle_label, "SPINDLE");
    lv_obj_set_style_text_font(status_spindle_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(status_spindle_label, UITheme::ACCENT_SECONDARY, 0);
    lv_obj_set_pos(status_spindle_label, 500, 250);
    
    lbl_modal_spindle = lv_label_create(tab);
    lv_label_set_text(lbl_modal_spindle, "M5");
    lv_obj_set_style_text_font(lbl_modal_spindle, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(lbl_modal_spindle, UITheme::UI_INFO, 0);
    lv_obj_set_pos(lbl_modal_spindle, 620, 250);

    // COOLANT
    lv_obj_t *status_coolant_label = lv_label_create(tab);
    lv_label_set_text(status_coolant_label, "COOLANT");
    lv_obj_set_style_text_font(status_coolant_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(status_coolant_label, UITheme::ACCENT_SECONDARY, 0);
    lv_obj_set_pos(status_coolant_label, 500, 279);
    
    lbl_modal_coolant = lv_label_create(tab);
    lv_label_set_text(lbl_modal_coolant, "M9");
    lv_obj_set_style_text_font(lbl_modal_coolant, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(lbl_modal_coolant, UITheme::UI_INFO, 0);
    lv_obj_set_pos(lbl_modal_coolant, 620, 279);

    // TOOL
    lv_obj_t *status_tool_label = lv_label_create(tab);
    lv_label_set_text(status_tool_label, "TOOL");
    lv_obj_set_style_text_font(status_tool_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(status_tool_label, UITheme::ACCENT_SECONDARY, 0);
    lv_obj_set_pos(status_tool_label, 500, 308);
    
    lbl_modal_tool = lv_label_create(tab);
    lv_label_set_text(lbl_modal_tool, "T0");
    lv_obj_set_style_text_font(lbl_modal_tool, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(lbl_modal_tool, UITheme::UI_WARNING, 0);
    lv_obj_set_pos(lbl_modal_tool, 620, 308);

    // FEED RATE & SPINDLE - Bottom row
    lv_obj_t *status_feed_header = lv_label_create(tab);
    lv_label_set_text(status_feed_header, "FEED RATE");
    lv_obj_set_style_text_font(status_feed_header, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(status_feed_header, UITheme::TEXT_DISABLED, 0);
    lv_obj_set_pos(status_feed_header, 15, 250);

    lbl_feed_value = lv_label_create(tab);
    lv_label_set_text(lbl_feed_value, "0 mm/min");
    lv_obj_set_style_text_font(lbl_feed_value, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(lbl_feed_value, lv_color_white(), 0);
    lv_obj_set_pos(lbl_feed_value, 15, 270);
    
    lbl_feed_override = lv_label_create(tab);
    lv_label_set_text(lbl_feed_override, "Override: 100%");
    lv_obj_set_style_text_font(lbl_feed_override, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(lbl_feed_override, UITheme::UI_INFO, 0);
    lv_obj_set_pos(lbl_feed_override, 15, 300);

    lv_obj_t *status_speed_header = lv_label_create(tab);
    lv_label_set_text(status_speed_header, "SPINDLE");
    lv_obj_set_style_text_font(status_speed_header, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(status_speed_header, UITheme::TEXT_DISABLED, 0);
    lv_obj_set_pos(status_speed_header, 250, 250);

    lbl_spindle_value = lv_label_create(tab);
    lv_label_set_text(lbl_spindle_value, "0 RPM");
    lv_obj_set_style_text_font(lbl_spindle_value, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(lbl_spindle_value, lv_color_white(), 0);
    lv_obj_set_pos(lbl_spindle_value, 250, 270);
    
    lbl_spindle_override = lv_label_create(tab);
    lv_label_set_text(lbl_spindle_override, "Override: 100%");
    lv_obj_set_style_text_font(lbl_spindle_override, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(lbl_spindle_override, UITheme::UI_INFO, 0);
    lv_obj_set_pos(lbl_spindle_override, 250, 300);
}

void UITabStatus::updateMessage(const char *message) {
    if (lbl_message) {
        lv_label_set_text(lbl_message, message);
        
        // Color code based on message prefix or keywords
        if (strncmp(message, "ERROR", 5) == 0 || strstr(message, "error") != nullptr) {
            lv_obj_set_style_text_color(lbl_message, UITheme::STATE_ALARM, 0);
        } else if (strncmp(message, "WARN", 4) == 0 || strstr(message, "warning") != nullptr) {
            lv_obj_set_style_text_color(lbl_message, UITheme::UI_WARNING, 0);
        } else if (strncmp(message, "OK", 2) == 0 || strstr(message, "success") != nullptr) {
            lv_obj_set_style_text_color(lbl_message, UITheme::UI_SUCCESS, 0);
        } else {
            lv_obj_set_style_text_color(lbl_message, UITheme::UI_INFO, 0);
        }
    }
}

void UITabStatus::updateState(const char *state) {
    if (!lbl_state) return;
    
    lv_label_set_text(lbl_state, state);
    
    // Update color based on state
    if (strcmp(state, "IDLE") == 0) {
        lv_obj_set_style_text_color(lbl_state, UITheme::STATE_IDLE, 0);
    } else if (strcmp(state, "RUN") == 0 || strcmp(state, "JOG") == 0) {
        lv_obj_set_style_text_color(lbl_state, UITheme::STATE_RUN, 0);
    } else if (strcmp(state, "ALARM") == 0) {
        lv_obj_set_style_text_color(lbl_state, UITheme::STATE_ALARM, 0);
    } else {
        lv_obj_set_style_text_color(lbl_state, UITheme::UI_WARNING, 0);
    }
}

void UITabStatus::updateWorkPosition(float x, float y, float z) {
    // Only update if values changed (avoid unnecessary redraws)
    if (x == last_wpos_x && y == last_wpos_y && z == last_wpos_z) {
        return;
    }
    
    char buf[20];
    
    if (lbl_wpos_x && x != last_wpos_x) {
        snprintf(buf, sizeof(buf), "X  %04.3f", x);
        lv_label_set_text(lbl_wpos_x, buf);
        last_wpos_x = x;
    }
    
    if (lbl_wpos_y && y != last_wpos_y) {
        snprintf(buf, sizeof(buf), "Y  %04.3f", y);
        lv_label_set_text(lbl_wpos_y, buf);
        last_wpos_y = y;
    }
    
    if (lbl_wpos_z && z != last_wpos_z) {
        snprintf(buf, sizeof(buf), "Z  %04.3f", z);
        lv_label_set_text(lbl_wpos_z, buf);
        last_wpos_z = z;
    }
}

void UITabStatus::updateMachinePosition(float x, float y, float z) {
    // Only update if values changed (avoid unnecessary redraws)
    if (x == last_mpos_x && y == last_mpos_y && z == last_mpos_z) {
        return;
    }
    
    char buf[20];
    
    if (lbl_mpos_x && x != last_mpos_x) {
        snprintf(buf, sizeof(buf), "X  %04.3f", x);
        lv_label_set_text(lbl_mpos_x, buf);
        last_mpos_x = x;
    }
    
    if (lbl_mpos_y && y != last_mpos_y) {
        snprintf(buf, sizeof(buf), "Y  %04.3f", y);
        lv_label_set_text(lbl_mpos_y, buf);
        last_mpos_y = y;
    }
    
    if (lbl_mpos_z && z != last_mpos_z) {
        snprintf(buf, sizeof(buf), "Z  %04.3f", z);
        lv_label_set_text(lbl_mpos_z, buf);
        last_mpos_z = z;
    }
}

void UITabStatus::updateFeedRate(float rate, float override_pct) {
    // Only update if values changed
    if (rate == last_feed_rate && override_pct == last_feed_override) {
        return;
    }
    
    char buf[32];
    
    if (lbl_feed_value && rate != last_feed_rate) {
        snprintf(buf, sizeof(buf), "%.0f mm/min", rate);
        lv_label_set_text(lbl_feed_value, buf);
        last_feed_rate = rate;
    }
    
    if (lbl_feed_override && override_pct != last_feed_override) {
        snprintf(buf, sizeof(buf), "Override: %.0f%%", override_pct);
        lv_label_set_text(lbl_feed_override, buf);
        last_feed_override = override_pct;
    }
}

void UITabStatus::updateSpindle(float speed, float override_pct) {
    // Only update if values changed
    if (speed == last_spindle_speed && override_pct == last_spindle_override) {
        return;
    }
    
    char buf[32];
    
    if (lbl_spindle_value && speed != last_spindle_speed) {
        snprintf(buf, sizeof(buf), "%.0f RPM", speed);
        lv_label_set_text(lbl_spindle_value, buf);
        last_spindle_speed = speed;
    }
    
    if (lbl_spindle_override && override_pct != last_spindle_override) {
        snprintf(buf, sizeof(buf), "Override: %.0f%%", override_pct);
        lv_label_set_text(lbl_spindle_override, buf);
        last_spindle_override = override_pct;
    }
}

void UITabStatus::updateModalStates(const char *wcs, const char *plane, const char *dist, 
                                    const char *units, const char *motion, const char *spindle,
                                    const char *coolant, const char *tool) {
    if (lbl_modal_wcs) {
        lv_label_set_text(lbl_modal_wcs, wcs);
    }
    
    if (lbl_modal_plane) {
        lv_label_set_text(lbl_modal_plane, plane);
    }
    
    if (lbl_modal_dist) {
        lv_label_set_text(lbl_modal_dist, dist);
    }
    
    if (lbl_modal_units) {
        lv_label_set_text(lbl_modal_units, units);
    }
    
    if (lbl_modal_motion) {
        lv_label_set_text(lbl_modal_motion, motion);
    }
    
    if (lbl_modal_spindle) {
        lv_label_set_text(lbl_modal_spindle, spindle);
    }
    
    if (lbl_modal_coolant) {
        lv_label_set_text(lbl_modal_coolant, coolant);
    }
    
    if (lbl_modal_tool) {
        lv_label_set_text(lbl_modal_tool, tool);
    }
}
