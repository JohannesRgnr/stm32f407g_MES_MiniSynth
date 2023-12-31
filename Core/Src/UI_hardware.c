/**
 * @file UI_hardware.c
 * @author johannes regnier
 * @brief hardware user interface
 * *****  UI is for the moment:
 * *****  3 potentiometers (on PC4. PC5 and PC2).. Data resolution is 10bits [0, 1023], smoothed using a lowpass filter
 * *****  1 encoder (PB4 / PB5) 
 * @version 0.1
 * @date 2024-01-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "UI_hardware.h"
#include "adc.h"
#include "filters.h"
#include "SEGGER_RTT.h"
#include "helper_functions.h"
#include "tim.h"
#include "UI_LCD.h"
#include "LCDController.h"
#include "st7789v.h"

uint16_t raw1, raw2, raw3;
extern onepoleLP_t smooth_ADC1;
extern onepoleLP_t smooth_ADC2;
extern onepoleLP_t smooth_ADC3;
static uint16_t data_pot1, data_pot2, data_pot3;
float pot1_norm, pot2_norm, pot3_norm;
uint16_t old_count = 0, count = 0, current_count;
int8_t enc_diff = 0;
uint16_t old_btn = 1, current_btn = 1;
uint8_t btn_pressed;
extern lv_obj_t *tabview;

/**
 * @brief Polling ADC values. This function is called by SysTick_Handler (in stm32f4xx_it.c) every ms.
 * 
 */
void poll_ADCs(void){
    HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	raw1 = HAL_ADC_GetValue(&hadc1);
	data_pot1 = (uint16_t)smoothing_LP(&smooth_ADC1, HAL_ADC_GetValue(&hadc1), 0.98) >> 2;
	pot1_norm = data_pot1 * ONE_OVER_1023;

	HAL_ADC_Start(&hadc2);
	HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY);
	raw2 = HAL_ADC_GetValue(&hadc2);
	data_pot2 = (uint16_t)smoothing_LP(&smooth_ADC2, HAL_ADC_GetValue(&hadc2), 0.98) >> 2;
	pot2_norm = data_pot2 * ONE_OVER_1023;

	HAL_ADC_Start(&hadc3);
	HAL_ADC_PollForConversion(&hadc3, HAL_MAX_DELAY);
	raw3 = HAL_ADC_GetValue(&hadc3);
	data_pot3 = (uint16_t)smoothing_LP(&smooth_ADC3, HAL_ADC_GetValue(&hadc3), 0.98) >> 2;
	pot3_norm = data_pot3 * ONE_OVER_1023;

	// SEGGER_RTT_printf(0, "Pots values [0, 1023] : %u %u\r\n", data_pot1, data_pot2); // debug
}

/**
 * @brief poll encoder.  This function is called by SysTick_Handler (in stm32f4xx_it.c) every ms.
 * 
 */
void poll_Encoder(void){
	// static old_count;
	count = ((TIM3->CNT) >> 2);
	if (count != old_count){
		// enc_diff = count - old_count;
		current_count = count;
		old_count = count;
		// SEGGER_RTT_printf(0, "Encoder count = %d\r\n", current_count); // debug
		// SEGGER_RTT_printf(0, "Encoder diff = %d\r\n", enc_diff); // debug
		// return enc_diff;
	}
	
}


/**
 * @brief poll encoder button.  This function is called by SysTick_Handler (in stm32f4xx_it.c) every ms.
 * not used at the moment.. TODO!
 */
void poll_EncoderBtn(void){
	current_btn = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7);
	if (current_btn != old_btn && old_btn == 0){
		btn_pressed = 1;
		//SEGGER_RTT_printf(0, "Encoder button released!\r\n"); // debug
	} else
		btn_pressed = 0;
	old_btn = current_btn;
}
