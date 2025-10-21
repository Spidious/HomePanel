#ifndef UI_TAB_CONTROL_JOG_H
#define UI_TAB_CONTROL_JOG_H

#include <lvgl.h>

class UITabControlJog {
public:
    static void create(lv_obj_t *tab);

private:
    static lv_obj_t *xy_step_display_label;
    static lv_obj_t *z_step_display_label;
    static lv_obj_t *xy_step_buttons[6];
    static lv_obj_t *z_step_buttons[3];
    static lv_obj_t *xy_feedrate_label;
    static lv_obj_t *z_feedrate_label;
    static float xy_current_step;
    static float z_current_step;
    static int xy_current_step_index;
    static int z_current_step_index;
    static void xy_step_button_event_cb(lv_event_t *e);
    static void z_step_button_event_cb(lv_event_t *e);
    static void xy_feedrate_adj_event_cb(lv_event_t *e);
    static void z_feedrate_adj_event_cb(lv_event_t *e);
    static void update_xy_step_display();
    static void update_z_step_display();
    static void update_xy_step_button_styles();
    static void update_z_step_button_styles();
};

#endif // UI_TAB_CONTROL_JOG_H
