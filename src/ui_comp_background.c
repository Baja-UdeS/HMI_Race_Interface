// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.5.1
// LVGL version: 8.3.6
// Project name: BJ25 Dash Display

#include "ui.h"

// COMPONENT Background

lv_obj_t * ui_Background_create(lv_obj_t * comp_parent)
{

    lv_obj_t * cui_Background;
    cui_Background = lv_img_create(comp_parent);
    lv_obj_set_width(cui_Background, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(cui_Background, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(cui_Background, LV_ALIGN_CENTER);
    lv_obj_add_flag(cui_Background, LV_OBJ_FLAG_CLICKABLE);     /// Flags
    lv_obj_clear_flag(cui_Background, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    lv_obj_t ** children = lv_mem_alloc(sizeof(lv_obj_t *) * _UI_COMP_BACKGROUND_NUM);
    children[UI_COMP_BACKGROUND_BACKGROUND] = cui_Background;
    lv_obj_add_event_cb(cui_Background, get_component_child_event_cb, LV_EVENT_GET_COMP_CHILD, children);
    lv_obj_add_event_cb(cui_Background, del_component_child_event_cb, LV_EVENT_DELETE, children);
    ui_comp_Background_create_hook(cui_Background);
    return cui_Background;
}

