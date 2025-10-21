#ifndef UI_TAB_TERMINAL_H
#define UI_TAB_TERMINAL_H

#include <lvgl.h>

class UITabTerminal {
public:
    static void create(lv_obj_t *tab);

private:
    static lv_obj_t *terminal_text;
    static lv_obj_t *input_field;
    static lv_obj_t *keyboard;
    
    static void send_button_event_cb(lv_event_t *e);
    static void input_field_event_cb(lv_event_t *e);
    static void keyboard_event_cb(lv_event_t *e);
    static void send_command();
};

#endif // UI_TAB_TERMINAL_H
