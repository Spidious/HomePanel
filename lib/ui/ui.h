#ifndef UI_H
#define UI_H

#include "lvgl.h"

// Assert color depth must remain 16
#if LV_COLOR_DEPTH != 16
#error "LV_COLOR_DEPTH should be 16bit"
#endif

#include "screens/home_page.h"

void ui_init(void);
void ui_destroy(void);

#endif // UI_H