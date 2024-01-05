
/* TODO ... need linear or allpass interpolation .... */



#include "flanger.h"
#include "oscillators.h"

static float	flanger_buff[FLANGER_BUFF_SIZE + 2]; 	 
static float    *FLreadptr, *FLwriteptr;
static float    FL_feedback = INIT_FL_FEEDB;
static float    flanger_wet  = INIT_FLANGER_WET;
static uint16_t		fl_delay_time;

extern oscillator_t flanger_lfo;

void flanger_init(void)
{
	fl_delay_time = INIT_FL_DEL;
	FLreadptr = flanger_buff;
	FLwriteptr = flanger_buff + INIT_FL_DEL;
	osc_init(&flanger_lfo, 0.5, 1, 0, 0, 0.5);
}





float flanger_compute (float input_sample)
{
	float delayed_sample, sample;
	float 	*pos;

	flanger_lfo.freq = 0.1;
	fl_delay_time = osc_Sine(&flanger_lfo) * 200 + 100;
	fl_delay_time = clip(fl_delay_time, 1, FLANGER_BUFF_SIZE - 1);
	pos = FLwriteptr - fl_delay_time;
	if (pos >= flanger_buff)
		FLreadptr = pos;
	else
		FLreadptr = pos + FLANGER_BUFF_SIZE - 1;

	

	// read first so that we can have feedback, apply lowpass filtering
	delayed_sample = *FLreadptr;
	
	// apply soft clipping 
	sample = SoftClip(input_sample + FL_feedback * delayed_sample);

	// write then update pointers
	*FLwriteptr = sample; 
	FLwriteptr++;
	FLreadptr++;
	


	
	if ((FLwriteptr - flanger_buff) >= FLANGER_BUFF_SIZE)
		FLwriteptr = flanger_buff;

	if ((FLreadptr - flanger_buff) >= FLANGER_BUFF_SIZE)
		FLreadptr = flanger_buff;

	

	

	return (flanger_wet * sample + (1 - flanger_wet) * input_sample); // linear crossfade
}