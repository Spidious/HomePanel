#ifndef UI_TAB_STATUS_H
#define UI_TAB_STATUS_H

#include <lvgl.h>

class UITabStatus {
public:
    static void create(lv_obj_t *tab);
    
    // Update methods for live data
    static void updateMessage(const char *message);
    static void updateState(const char *state);
    static void updateWorkPosition(float x, float y, float z);
    static void updateMachinePosition(float x, float y, float z);
    static void updateFeedRate(float rate, float override_pct);
    static void updateSpindle(float speed, float override_pct);
    static void updateModalStates(const char *wcs, const char *plane, const char *dist, 
                                  const char *units, const char *motion, const char *spindle,
                                  const char *coolant, const char *tool);
    
private:
    static lv_obj_t *lbl_message;
    static lv_obj_t *lbl_state;
    static lv_obj_t *lbl_wpos_x;
    static lv_obj_t *lbl_wpos_y;
    static lv_obj_t *lbl_wpos_z;
    static lv_obj_t *lbl_mpos_x;
    static lv_obj_t *lbl_mpos_y;
    static lv_obj_t *lbl_mpos_z;
    static lv_obj_t *lbl_feed_value;
    static lv_obj_t *lbl_feed_override;
    static lv_obj_t *lbl_spindle_value;
    static lv_obj_t *lbl_spindle_override;
    
    // Modal state labels
    static lv_obj_t *lbl_modal_wcs;
    static lv_obj_t *lbl_modal_plane;
    static lv_obj_t *lbl_modal_dist;
    static lv_obj_t *lbl_modal_units;
    static lv_obj_t *lbl_modal_motion;
    static lv_obj_t *lbl_modal_spindle;
    static lv_obj_t *lbl_modal_coolant;
    static lv_obj_t *lbl_modal_tool;
    
    // Cached values for delta checking (prevent unnecessary redraws)
    static float last_wpos_x, last_wpos_y, last_wpos_z;
    static float last_mpos_x, last_mpos_y, last_mpos_z;
    static float last_feed_rate, last_feed_override;
    static float last_spindle_speed, last_spindle_override;
};

#endif // UI_TAB_STATUS_H
