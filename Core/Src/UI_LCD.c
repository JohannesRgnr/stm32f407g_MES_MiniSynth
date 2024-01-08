/**
 * @file UI_LCD.c
 * @author johannes regnier
 * @brief A simple graphical user interface. Basic for now.
 * @version 0.1
 * @date 2024-01-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "UI_LCD.h"

#include "st7789v.h"
#include "tim.h"
#include "SEGGER_RTT.h"





extern float pot1_norm, pot2_norm, pot3_norm;
extern uint16_t current_count, current_btn;
extern int8_t enc_diff;
extern uint8_t btn_pressed;

lv_obj_t *pot1, *pot2, *pot3, *tabview;
lv_group_t* g; 
lv_indev_t * my_indev;
lv_obj_t * osc_txt;

/**
 * @brief Create the 3 arcs displaying the states of the 3 potentiometers, with labels
 * 
 */
void create_arcs(void){
    /* Create text*/
    lv_obj_t * title = lv_label_create(lv_scr_act());
    lv_label_set_text(title, "MES MiniSynth");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);


     /* Create text*/
    osc_txt = lv_label_create(lv_scr_act());
    lv_label_set_text(osc_txt, "virtual analog");
    lv_obj_set_style_text_font(osc_txt, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_align(osc_txt, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(osc_txt, LV_ALIGN_TOP_MID, 0, 40);


    /*Create Arcs for the 3 pots*/
    pot1 = lv_arc_create(lv_scr_act());
    lv_obj_set_size(pot1, 72, 72);
	lv_obj_align(pot1, LV_ALIGN_CENTER, -90, -10);
    pot2 = lv_arc_create(lv_scr_act());
    lv_obj_set_size(pot2, 72, 72);
	lv_obj_align(pot2, LV_ALIGN_CENTER, 90, -10);
    pot3 = lv_arc_create(lv_scr_act());
    lv_obj_set_size(pot3, 72, 72);
	lv_obj_align(pot3, LV_ALIGN_CENTER, 0, 65);
   
    lv_arc_set_rotation(pot1, 135);
    lv_arc_set_bg_angles(pot1, 0, 270);
    lv_arc_set_rotation(pot2, 135);
    lv_arc_set_bg_angles(pot2, 0, 270);
    lv_arc_set_rotation(pot3, 135);
    lv_arc_set_bg_angles(pot3, 0, 270);
    
    /*Make them unclickable, and without knob*/
    lv_obj_remove_style(pot1, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(pot1, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_style(pot2, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(pot2, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_style(pot3, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(pot3, LV_OBJ_FLAG_CLICKABLE);
    

    // /*Create labels for potentiometers*/
    lv_obj_t * label1 = lv_label_create(lv_scr_act());
    lv_label_set_text_static(label1, "cutoff");

    lv_obj_t * label2 = lv_label_create(lv_scr_act());
    lv_label_set_text_static(label2, "decay");

    lv_obj_t * label3 = lv_label_create(lv_scr_act());
    lv_label_set_text_static(label3, "delay");
    
    lv_obj_align_to(label1, pot1, LV_ALIGN_BOTTOM_MID, 0, +15);
    lv_obj_align_to(label2, pot2, LV_ALIGN_BOTTOM_MID, 0, +15);
    lv_obj_align_to(label3, pot3, LV_ALIGN_BOTTOM_MID, 0, +15);
    
}

// TODO: use tabs for oscillators/ filter / amp / fx
void create_tabs(void){
      /*Create a Tab view object*/
   
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

    

}

// TODO : GUI includes encoder

// static int32_t encoder_last_value;
// static lv_indev_state_t encoder_state;

// static bool encoder_read(lv_indev_drv_t * drv, lv_indev_data_t*data){
//     static int32_t last_diff = 0;
    

//     static int lastBtn;
//     int btn_state;

//     if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == 0) {
//         btn_state = LV_INDEV_STATE_PR;
//     } else {
//         btn_state = LV_INDEV_STATE_REL;
//     }
//     int encoder_value = ((TIM3->CNT)  >> 2);
//     encoder_state = btn_state;
//     data->enc_diff = poll_Encoder();
//     data->state = encoder_state;
//     encoder_last_value = encoder_value;



//     // int32_t diff = ((TIM3->CNT) >> 2); // read the rotary encoder position
//     // int btn_state = 0 ==  HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL; // read encoder push button status

//     // data->enc_diff = diff - last_diff;;
//     // data->state = btn_state;
   
//     SEGGER_RTT_printf(0, "Encoder diff = %d\r\n", data->enc_diff); // debug
//     SEGGER_RTT_printf(0, "btn_state = %d\r\n", btn_state); // debug

//     if (lastBtn != btn_state)
//     {
//         lastBtn = btn_state;
//     }

//     return false;
// }


// static void event_handler(lv_event_t * e)
// {
//     lv_event_code_t code = lv_event_get_code(e);
//     lv_obj_t * obj = lv_event_get_target(e);
//     if(code == LV_EVENT_VALUE_CHANGED) {
//         char buf[32];
//         lv_roller_get_selected_str(obj, buf, sizeof(buf));
//         LV_LOG_USER("Selected month: %s\n", buf);
//     }
// }

void UI_LCD_init(void){

    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x003a57), LV_PART_MAIN);
    lv_obj_set_style_text_color(lv_scr_act(), lv_color_hex(0xffffff), LV_PART_MAIN);
    create_arcs();

    
}


/**
 * @brief display the state of the 3 potentiometers, and the current oscillator mode
 * 
 */
void UI_LCD_process(void){
    lv_arc_set_angles(pot1, 0, 270 * pot1_norm);
    lv_arc_set_angles(pot2, 0, 270 * pot2_norm); 
    lv_arc_set_angles(pot3, 0, 270 * pot3_norm);
    uint8_t osc_select = current_count % 2;
    switch(osc_select){
			case 0:
			// analog waveforms
                lv_label_set_text(osc_txt, "virtual analog");
            break;

            case 1:
			// FM
				lv_label_set_text(osc_txt, "FM 2 operators");
				break;
		}

}
