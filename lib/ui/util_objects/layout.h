#pragma once
#include <lvgl.h>

class Layout
{
protected:
    // This layout
    lv_obj_t* parent;

public:
    Layout(lv_obj_t* parent): parent(parent) {}
    virtual ~Layout() {}

    virtual void build_layout() = 0;

    void show()
    {
        // build the layout
        this->hide();
        build_layout();
    }

    void hide() {
        lv_obj_clean(parent);
    }

};