/**
 * @file stereo_delay.h
 * @author johannes regnier, modified from Xavier Halgand
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
#define DELAY_BUFF_SIZE         8192  // max delay in samples 
#define INIT_DELAY_L            6000  // init delay (in samples)
#define INIT_DELAY_R            8000  // init delay (in samples)
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
void 	Delay_clean(void);
void 	Delay_feedback_inc(void);
void	Delay_feedback_dec(void);
void 	DelayFeedback_set(uint8_t val);
void 	DelayWet_set(uint8_t val);
void 	Delay_time_inc(uint8_t val);
void	Delay_time_dec(uint8_t val);
void 	Delay_time_set(uint8_t val);
void    pingpongDelay_compute(float x, float *delayLOut, float *delayROut);


//*************************************************************************************************
#endif
