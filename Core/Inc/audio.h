/**
 * @file audio.h
 * @author johannes regnier
 * @brief 
 * @version 0.1
 * @date 2023-12-30
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef AUDIO_H
#define AUDIO_H

#include "../../Drivers/BSP/STM32F4-Discovery/stm32f4_discovery_audio.h"
#include "CONSTS.h"
#include <stdint.h>




extern I2S_HandleTypeDef hAudioOutI2s;
extern I2S_HandleTypeDef hAudioInI2s;

struct lino
{
	float val;		// current value
	float dst;		// value to move to
	float inc;		// increment per sample to get from val to dst
};



/**
 * @brief Init audio and start audio 
 * 
 */
void AUDIO_Init();


/**
 * @brief Main audio function. This is where the audio samples are computed, then the audio buffer is filled
 * 
 * @param input 
 * @param output 
 * @param samples 
 */
void audioBlock(int16_t *output, uint16_t samples);


#endif // !AUDIO_H