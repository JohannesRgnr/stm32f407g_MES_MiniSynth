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

//*************************************************************************************************

/*--------------------- Global Defines ------------------------ */
#define DELAY_BUFF_SIZE         11000  // max delay in samples 
#define INIT_DELAY_L            10000  // init delay (in samples)
#define INIT_DELAY_R            10000  // init delay (in samples)
#define INIT_FEEDB              0.9f
#define INIT_DELAY_WET			0.5f
#define MIN_DELAY				250		// min delay, in samples
#define DELTA_DELAY				20		// variation step of delay time

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "CONSTS.h"
#include "helper_functions.h"
#include "filters.h"



/* Exported functions --------------------------------------------------------*/

void 	Delay_init(void);
void 	DelayFeedback_set(uint8_t val);
void 	DelayWet_set(uint8_t val);
void 	Delay_time_set(uint8_t val);
void    pingpongDelay_compute(float x, float *delayLOut, float *delayROut);


//*************************************************************************************************
#endif
