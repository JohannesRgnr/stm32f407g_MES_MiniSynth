
#include "UI_LCD.h"
#include "LCDController.h"
#include "st7789v.h"


static void value_changed_event_cb(lv_event_t * e);
extern float pot1_norm, pot2_norm;

lv_obj_t *pot1, *pot2;

void UI_LCD_init(){
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x003a57), LV_PART_MAIN);
	lv_obj_set_style_text_color(lv_scr_act(), lv_color_hex(0xffffff), LV_PART_MAIN);

    /*Create arcs for potentiometers*/
    lv_obj_t * label1 = lv_label_create(lv_scr_act());
    lv_label_set_text_static(label1, "cutoff");

    lv_obj_t * label2 = lv_label_create(lv_scr_act());
    lv_label_set_text_static(label2, "resonance");

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
    
    

    lv_obj_remove_style(pot1, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(pot1, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_style(pot2, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(pot2, LV_OBJ_FLAG_CLICKABLE);
    
    lv_obj_align_to(label1, pot1, LV_ALIGN_BOTTOM_MID, 0, +15);
    lv_obj_align_to(label2, pot2, LV_ALIGN_BOTTOM_MID, 0, +15);

        //lv_obj_add_event_cb(arc, value_changed_event_cb, LV_EVENT_VALUE_CHANGED, label);

    /*Manually update the label for the first time*/
    // lv_event_send(pot1, LV_EVENT_VALUE_CHANGED, NULL);

    /*Create a spinner*/
	// lv_obj_t * spinner = lv_spinner_create(lv_scr_act(), 1000, 60);
	// lv_obj_set_size(spinner, 64, 64);
	// lv_obj_align(spinner, LV_ALIGN_BOTTOM_MID, 0, 0);
}

void UI_LCD_process(){
    lv_arc_set_angles(pot1, 0, 270 * pot1_norm);
    lv_arc_set_angles(pot2, 0, 270 * pot2_norm);
}

// static void value_changed_event_cb(lv_event_t * e)
// {
//     lv_obj_t * arc = lv_event_get_target(e);
//     lv_obj_t * label = lv_event_get_user_data(e);

//     lv_label_set_text_fmt(label, "%d%%", pot1_norm * 1023);

//     /*Rotate the label to the current position of the arc*/
//     lv_arc_rotate_obj_to_angle(arc, label, arc);
// }
