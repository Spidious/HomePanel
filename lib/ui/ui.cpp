#include "ui.h"

lv_obj_t * ui____initial_actions0;

void ui_init(void)
{
    lv_disp_t* disp = lv_display_get_default();

    lv_theme_t* theme = lv_theme_default_init(
        disp,
        lv_palette_main(LV_PALETTE_BLUE),
        lv_palette_main(LV_PALETTE_RED),
        true,
        LV_FONT_DEFAULT
    );

    lv_disp_set_theme(disp, theme);

    lv_obj_t* root = lv_disp_get_scr_act(disp);

    Homepage home(root);

    // show home first
    home.show();

}

void ui_destroy(void)
{
}