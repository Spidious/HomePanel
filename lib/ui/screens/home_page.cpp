#include "screens/home_page.h"
#include <LittleFS.h>
#include <lvgl.h>

void Homepage::build_layout()
{
    lv_obj_t * img = lv_img_create(lv_scr_act());
    lv_image_set_src(img, "S:/assets/ui_default_img.bin");
    lv_obj_align(img, LV_ALIGN_RIGHT_MID, -20, 0);

}
