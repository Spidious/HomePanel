#include "ui/tabs/control/ui_tab_control_actions.h"
#include "ui/ui_theme.h"

void UITabControlActions::create(lv_obj_t *tab) {
    const int col_width = 180;
    const int col_spacing = 20;
    const int left_col_x = 10;
    const int middle_col_x = left_col_x + col_width + col_spacing;
    const int right_col_x = middle_col_x + col_width + col_spacing;
    
    // ========== LEFT COLUMN: Control Buttons ==========
    lv_obj_t *control_label = lv_label_create(tab);
    lv_label_set_text(control_label, "Control:");
    lv_obj_set_style_text_font(control_label, &lv_font_montserrat_18, 0);
    lv_obj_set_pos(control_label, left_col_x, 10);
    
    lv_obj_t *btn_pause = lv_button_create(tab);
    lv_obj_set_size(btn_pause, col_width, 60);
    lv_obj_set_pos(btn_pause, left_col_x, 45);
    lv_obj_t *lbl_pause = lv_label_create(btn_pause);
    lv_label_set_text(lbl_pause, "Pause");
    lv_obj_set_style_text_font(lbl_pause, &lv_font_montserrat_18, 0);
    lv_obj_center(lbl_pause);
    
    lv_obj_t *btn_unlock = lv_button_create(tab);
    lv_obj_set_size(btn_unlock, col_width, 60);
    lv_obj_set_pos(btn_unlock, left_col_x, 115);
    lv_obj_t *lbl_unlock = lv_label_create(btn_unlock);
    lv_label_set_text(lbl_unlock, "Unlock");
    lv_obj_set_style_text_font(lbl_unlock, &lv_font_montserrat_18, 0);
    lv_obj_center(lbl_unlock);
    
    lv_obj_t *btn_reset = lv_button_create(tab);
    lv_obj_set_size(btn_reset, col_width, 60);
    lv_obj_set_pos(btn_reset, left_col_x, 185);
    lv_obj_t *lbl_reset = lv_label_create(btn_reset);
    lv_label_set_text(lbl_reset, "Soft Reset");
    lv_obj_set_style_text_font(lbl_reset, &lv_font_montserrat_18, 0);
    lv_obj_center(lbl_reset);
    
    // ========== MIDDLE COLUMN: Home Axis ==========
    lv_obj_t *home_label = lv_label_create(tab);
    lv_label_set_text(home_label, "Home Axis:");
    lv_obj_set_style_text_font(home_label, &lv_font_montserrat_18, 0);
    lv_obj_set_pos(home_label, middle_col_x, 10);
    
    // Home X
    lv_obj_t *btn_home_x = lv_button_create(tab);
    lv_obj_set_size(btn_home_x, col_width, 50);
    lv_obj_set_pos(btn_home_x, middle_col_x, 45);
    lv_obj_set_style_bg_color(btn_home_x, UITheme::AXIS_X, LV_PART_MAIN);
    lv_obj_t *lbl_home_x = lv_label_create(btn_home_x);
    lv_label_set_text(lbl_home_x, "Home X");
    lv_obj_set_style_text_font(lbl_home_x, &lv_font_montserrat_16, 0);
    lv_obj_center(lbl_home_x);
    
    // Home Y
    lv_obj_t *btn_home_y = lv_button_create(tab);
    lv_obj_set_size(btn_home_y, col_width, 50);
    lv_obj_set_pos(btn_home_y, middle_col_x, 105);
    lv_obj_set_style_bg_color(btn_home_y, UITheme::AXIS_Y, LV_PART_MAIN);
    lv_obj_t *lbl_home_y = lv_label_create(btn_home_y);
    lv_label_set_text(lbl_home_y, "Home Y");
    lv_obj_set_style_text_font(lbl_home_y, &lv_font_montserrat_16, 0);
    lv_obj_center(lbl_home_y);
    
    // Home Z
    lv_obj_t *btn_home_z = lv_button_create(tab);
    lv_obj_set_size(btn_home_z, col_width, 50);
    lv_obj_set_pos(btn_home_z, middle_col_x, 165);
    lv_obj_set_style_bg_color(btn_home_z, UITheme::AXIS_Z, LV_PART_MAIN);
    lv_obj_t *lbl_home_z = lv_label_create(btn_home_z);
    lv_label_set_text(lbl_home_z, "Home Z");
    lv_obj_set_style_text_font(lbl_home_z, &lv_font_montserrat_16, 0);
    lv_obj_center(lbl_home_z);
    
    // Home All
    lv_obj_t *btn_home_all = lv_button_create(tab);
    lv_obj_set_size(btn_home_all, col_width, 50);
    lv_obj_set_pos(btn_home_all, middle_col_x, 225);
    lv_obj_set_style_bg_color(btn_home_all, UITheme::AXIS_XY, LV_PART_MAIN);
    lv_obj_t *lbl_home_all = lv_label_create(btn_home_all);
    lv_label_set_text(lbl_home_all, "Home All");
    lv_obj_set_style_text_font(lbl_home_all, &lv_font_montserrat_16, 0);
    lv_obj_center(lbl_home_all);
    
    // ========== RIGHT COLUMN: Zero Axis ==========
    lv_obj_t *zero_label = lv_label_create(tab);
    lv_label_set_text(zero_label, "Zero Axis:");
    lv_obj_set_style_text_font(zero_label, &lv_font_montserrat_18, 0);
    lv_obj_set_pos(zero_label, right_col_x, 10);
    
    // Zero X
    lv_obj_t *btn_zero_x = lv_button_create(tab);
    lv_obj_set_size(btn_zero_x, col_width, 50);
    lv_obj_set_pos(btn_zero_x, right_col_x, 45);
    lv_obj_set_style_bg_color(btn_zero_x, UITheme::AXIS_X, LV_PART_MAIN);
    lv_obj_t *lbl_zero_x = lv_label_create(btn_zero_x);
    lv_label_set_text(lbl_zero_x, "Zero X");
    lv_obj_set_style_text_font(lbl_zero_x, &lv_font_montserrat_16, 0);
    lv_obj_center(lbl_zero_x);
    
    // Zero Y
    lv_obj_t *btn_zero_y = lv_button_create(tab);
    lv_obj_set_size(btn_zero_y, col_width, 50);
    lv_obj_set_pos(btn_zero_y, right_col_x, 105);
    lv_obj_set_style_bg_color(btn_zero_y, UITheme::AXIS_Y, LV_PART_MAIN);
    lv_obj_t *lbl_zero_y = lv_label_create(btn_zero_y);
    lv_label_set_text(lbl_zero_y, "Zero Y");
    lv_obj_set_style_text_font(lbl_zero_y, &lv_font_montserrat_16, 0);
    lv_obj_center(lbl_zero_y);
    
    // Zero Z
    lv_obj_t *btn_zero_z = lv_button_create(tab);
    lv_obj_set_size(btn_zero_z, col_width, 50);
    lv_obj_set_pos(btn_zero_z, right_col_x, 165);
    lv_obj_set_style_bg_color(btn_zero_z, UITheme::AXIS_Z, LV_PART_MAIN);
    lv_obj_t *lbl_zero_z = lv_label_create(btn_zero_z);
    lv_label_set_text(lbl_zero_z, "Zero Z");
    lv_obj_set_style_text_font(lbl_zero_z, &lv_font_montserrat_16, 0);
    lv_obj_center(lbl_zero_z);
    
    // Zero All
    lv_obj_t *btn_zero_all = lv_button_create(tab);
    lv_obj_set_size(btn_zero_all, col_width, 50);
    lv_obj_set_pos(btn_zero_all, right_col_x, 225);
    lv_obj_set_style_bg_color(btn_zero_all, UITheme::AXIS_XY, LV_PART_MAIN);
    lv_obj_t *lbl_zero_all = lv_label_create(btn_zero_all);
    lv_label_set_text(lbl_zero_all, "Zero All");
    lv_obj_set_style_text_font(lbl_zero_all, &lv_font_montserrat_16, 0);
    lv_obj_center(lbl_zero_all);

    // ========== QUICK STOP: Full Width at Bottom ==========
    lv_obj_t *btn_estop = lv_button_create(tab);
    lv_obj_set_size(btn_estop, col_width * 3 + col_spacing * 2, 60);
    lv_obj_set_pos(btn_estop, left_col_x, 290);
    lv_obj_set_style_bg_color(btn_estop, UITheme::BTN_ESTOP, LV_PART_MAIN);
    lv_obj_t *lbl_estop = lv_label_create(btn_estop);
    lv_label_set_text(lbl_estop, "QUICK STOP");
    lv_obj_set_style_text_font(lbl_estop, &lv_font_montserrat_20, 0);
    lv_obj_center(lbl_estop);
}
