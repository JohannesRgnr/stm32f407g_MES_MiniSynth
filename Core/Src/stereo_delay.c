/**
 * @file stereo_delay.h
 * @author johannes regnier, modified from Xavier Halgand
 * @brief stereo ping pon delay effect
 * @version 0.1
 * @date 2024-01-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "stereo_delay.h"


/*-------------------------------------------------------------------------------------------*/
/* Delay effect variables  */
static float		delaylineL[DELAY_BUFF_SIZE ];
static float		delaylineR[DELAY_BUFF_SIZE ];
static float		*readptrL _CCM_, *readptrR _CCM_; 	// output pointer of delay line
static float 		*writeptrL _CCM_, *writeptrR _CCM_; // input pointer of delay line
static uint16_t		delay_time_L _CCM_, delay_time_R _CCM_;


static float		feedback = INIT_FEEDB;
static float		delay_wet = INIT_DELAY_WET;

extern ZDFLP_t lp_L;
extern ZDFLP_t lp_R;

/*-------------------------------------------------------------------------------------------*/




void Delay_init(void)
{
	/* initialize pointers positions for delay effect */
	delay_time_L = INIT_DELAY_L;
	delay_time_R = INIT_DELAY_R;
	readptrL = delaylineL;
	readptrR = delaylineR;
	writeptrL = delaylineL + delay_time_L;
	writeptrR = delaylineR + delay_time_R;

	SVF_LP_init(&lp_L);
	SVF_LP_init(&lp_R);
}
/*-------------------------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------------------------*/
void Delay_time_set(uint8_t val)
{
	float 	*pos;
	delay_time_L = (uint32_t) lrintf((DELAY_BUFF_SIZE - 1 - MIN_DELAY)/MIDI_MAX * val + MIN_DELAY);
	pos = writeptrL - delay_time_L;
	if (pos >= delaylineL)
		readptrL = pos;
	else
		readptrL = pos + DELAY_BUFF_SIZE - 1;

}
/*-------------------------------------------------------------------------------------------*/
void Delay_feedback_inc(void)
{
	/* increment feedback delay */

	feedback *= 1.05f ;//
}
/*-------------------------------------------------------------------------------------------*/
void Delay_feedback_dec(void)
{
	/* decrement feedback delay */

	feedback *= 0.95f ;//
}
/*-------------------------------------------------------------------------------------------*/
void DelayFeedback_set(uint8_t val)
{
	feedback = val / MIDI_MAX;
}
/*-------------------------------------------------------------------------------------------*/
void Delaydelay_wet_set(uint8_t val)
{
	delay_wet = val / MIDI_MAX;
}
/*-------------------------------------------------------------------------------------------*/

/**
 * @brief Ping pong delay effect, with crossfeedback, softclip and lowpass filtering
 * @note requires 2 delay lines (L and R)
 * 
 * @param x 
 * @param delayLOut 
 * @param delayROut 
 */
void pingpongDelay_compute (float input_sample, float * delayLOut, float * delayROut)
{
	float delayed_sampleL, delayed_sampleR, sampleL, sampleR;
	float mask = DELAY_BUFF_SIZE - 1;
	// read first so that we can have feedback
	delayed_sampleL = SVF_LP_compute(&lp_L, *readptrL);
	delayed_sampleR = SVF_LP_compute(&lp_R, *readptrR);

	// apply soft clipping 
	sampleL = SoftClip(input_sample + feedback * delayed_sampleL);
	sampleR = SoftClip(input_sample + feedback * delayed_sampleR);

	// write then update pointers
	*writeptrL = sampleR; 
	*writeptrR = sampleL; 
	writeptrL++;
	readptrL++;
	writeptrR++;
	readptrR++;

	if ((writeptrL - delaylineL) >= DELAY_BUFF_SIZE)
		writeptrL = delaylineL;

	if ((readptrL - delaylineL) >= DELAY_BUFF_SIZE)
		readptrL = delaylineL;

	if ((writeptrR - delaylineR) >= DELAY_BUFF_SIZE)
		writeptrR = delaylineR;

	if ((readptrR - delaylineR) >= DELAY_BUFF_SIZE)
		readptrR = delaylineR;

	*delayLOut = (delay_wet * sampleL + (1 - delay_wet) * input_sample);
	*delayROut = (delay_wet * sampleR + (1 - delay_wet) * input_sample);	
}



