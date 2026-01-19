#ifndef HOME_PAGE_H
#define HOME_PAGE_H

#include "util_objects/layout.h"

// extern lv_image_dsc_t ui_default_img;

class Homepage : public Layout
{
public:
    Homepage(lv_obj_t * parent) : Layout(parent) {}

    void build_layout() override;
};

#endif // HOME_PAGE_H