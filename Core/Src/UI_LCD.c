
#include "UI_LCD.h"
#include "LCDController.h"
#include "st7789v.h"


static void value_changed_event_cb(lv_event_t * e);
extern float pot1_norm, pot2_norm;
extern float current_count;

lv_obj_t *pot1, *pot2, *tabview;


void create_arcs(void){
    /*Create 2 Arcs for the 2 pots*/
    pot1 = lv_arc_create(lv_scr_act());
    lv_obj_set_size(pot1, 80, 80);
	lv_obj_align(pot1, LV_ALIGN_CENTER, -70, 0);
    pot2 = lv_arc_create(lv_scr_act());
    lv_obj_set_size(pot2, 80, 80);
	lv_obj_align(pot2, LV_ALIGN_CENTER, 70, 0);
   
    lv_arc_set_rotation(pot1, 135);
    lv_arc_set_bg_angles(pot1, 0, 270);
    lv_arc_set_rotation(pot2, 135);
    lv_arc_set_bg_angles(pot2, 0, 270);
    
    /*Make them unclickable, and without knob*/
    lv_obj_remove_style(pot1, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(pot1, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_style(pot2, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(pot2, LV_OBJ_FLAG_CLICKABLE);

    // /*Create labels for potentiometers*/
    // lv_obj_t * label1 = lv_label_create(lv_scr_act());
    // lv_label_set_text_static(label1, "cutoff");

    // lv_obj_t * label2 = lv_label_create(lv_scr_act());
    // lv_label_set_text_static(label2, "resonance");
    
    // lv_obj_align_to(label1, pot1, LV_ALIGN_BOTTOM_MID, 0, +15);
    // lv_obj_align_to(label2, pot2, LV_ALIGN_BOTTOM_MID, 0, +15);
}


void create_tabs(void){
      /*Create a Tab view object*/
    lv_obj_t * tabview;
    tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 40);

    /*Add 3 tabs (the tabs are page (lv_page) and can be scrolled*/
    lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "osc");
    lv_obj_t * tab2 = lv_tabview_add_tab(tabview, "filter");
    lv_obj_t * tab3 = lv_tabview_add_tab(tabview, "delay");

    /*Add content to the tabs*/
    lv_obj_t * label1 = lv_label_create(tab1);
    lv_label_set_text_static(label1, "cutoff");

    lv_obj_t * label2 = lv_label_create(tab1);
    lv_label_set_text_static(label2, "resonance");
    
    lv_obj_align_to(label1, pot1, LV_ALIGN_BOTTOM_MID, 0, +15);
    lv_obj_align_to(label2, pot2, LV_ALIGN_BOTTOM_MID, 0, +15);;

    label1 = lv_label_create(tab2);
    lv_label_set_text(label1, "Second tab");

    label1 = lv_label_create(tab3);
    label2 = lv_label_create(tab3);
    lv_label_set_text_static(label1, "time");
    lv_label_set_text_static(label2, "feedback");

    //lv_obj_scroll_to_view_recursive(label, LV_ANIM_ON);
}

void UI_LCD_init(void){
    

    
    create_tabs();
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x003a57), LV_PART_MAIN);
	lv_obj_set_style_text_color(lv_scr_act(), lv_color_hex(0xffffff), LV_PART_MAIN);
    create_arcs();
}

void UI_LCD_process(void){
    lv_arc_set_angles(pot1, 0, 270 * pot1_norm);
    lv_arc_set_angles(pot2, 0, 270 * pot2_norm);    
}
