/**
 * @file stereo_delay.h
 * @author modified from Xavier Halgand
 * @brief 
 * @version 0.1
 * @date 2024-01-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef __STEREO_DELAY_H
#define __STEREO_DELAY_H


#include <stdint.h>
#include "CONSTS.h"
#include "helper_functions.h"
#include "filters.h"

#define DELAY_BUFF_SIZE         11025  
#define INIT_DELAY_L            11025  
#define INIT_DELAY_R            11025  
#define INIT_FEEDB              0.8f
#define INIT_DELAY_WET			0.4f





void 	Delay_init(void);
void 	DelayFeedback_set(uint8_t val);
void 	DelayWet_set(uint8_t val);
void    Delay_time_set(uint32_t time_L, uint32_t time_R);
void    pingpongDelay_compute(float x, float *delayLOut, float *delayROut);

#endif
