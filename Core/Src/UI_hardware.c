

#include "UI_hardware.h"
#include "adc.h"
#include "SEGGER_RTT.h"

uint16_t raw;

void poll_ADCs(void){
    HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	raw = HAL_ADC_GetValue(&hadc1);
	// raw = smoothing_filter(raw);
		
    SEGGER_RTT_printf(0, "Pot raw value [0, 4095] : %u.\r\n", raw);

}