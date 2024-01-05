/**
 * @file UI_hardware.c
 * @author johannes regnier
 * @brief hardware user interface
 * @note  UI is for the moment:
 * @note  2 potentiometers (on PC1 and PC2).. Data resolution is 10bits [0, 1023], smoothed using a lowpass filter`
 * @note  1 encoder (PB4 / PB5)
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

uint16_t raw1, raw2;
extern onepoleLP_t smooth_ADC1;
extern onepoleLP_t smooth_ADC2;
static uint16_t data_pot1, data_pot2;
float pot1_norm, pot2_norm;
uint16_t old_count, count, current_count;

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

	// SEGGER_RTT_printf(0, "Pots values [0, 1023] : %u %u\r\n", data_pot1, data_pot2); // debug
}

/**
 * @brief poll encoder.  This function is called by SysTick_Handler (in stm32f4xx_it.c) every ms.
 * 
 */
void poll_Encoder(void){
	count = ((TIM3->CNT) >> 2);
	if (count != old_count){
		current_count = count;
		// SEGGER_RTT_printf(0, "Encoder counter = %u.\r\n", count); // debug
	}
	old_count = count;
}
