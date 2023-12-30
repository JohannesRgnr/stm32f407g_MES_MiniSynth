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
#include "envelope.h"




extern I2S_HandleTypeDef hAudioOutI2s;
extern I2S_HandleTypeDef hAudioInI2s;


/**
 * @brief Init audio and start audio 
 * 
 */
void AUDIO_Init();


/**
 * @brief 
 * 
 * @param input 
 * @param output 
 * @param samples 
 */
void audioBlock(float *output, int32_t samples);


#endif // !AUDIO_H