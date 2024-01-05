
#include "UI_LCD.h"
#include "LCDController.h"
#include "st7789v.h"


static void value_changed_event_cb(lv_event_t * e);
extern float pot1_norm, pot2_norm;

lv_obj_t *pot1, *pot2;

void UI_LCD_init(){
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x003a57), LV_PART_MAIN);
	lv_obj_set_style_text_color(lv_scr_act(), lv_color_hex(0xffffff), LV_PART_MAIN);

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

    /*Create labels for potentiometers*/
    lv_obj_t * label1 = lv_label_create(lv_scr_act());
    lv_label_set_text_static(label1, "cutoff");

    lv_obj_t * label2 = lv_label_create(lv_scr_act());
    lv_label_set_text_static(label2, "resonance");
    
    lv_obj_align_to(label1, pot1, LV_ALIGN_BOTTOM_MID, 0, +15);
    lv_obj_align_to(label2, pot2, LV_ALIGN_BOTTOM_MID, 0, +15);

}

void UI_LCD_process(){
    lv_arc_set_angles(pot1, 0, 270 * pot1_norm);
    lv_arc_set_angles(pot2, 0, 270 * pot2_norm);
}


