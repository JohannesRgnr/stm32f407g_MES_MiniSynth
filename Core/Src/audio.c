/**
 * @file audio.c
 * @author johannes regnier
 * @brief Functions to make sound and main audio block function 
 * @note  Essential initialization functions from Tom Erbe:	
 * @note  http://synthnotes.ucsd.edu/wp4/index.php/2019/09/24/setting-up-the-stmf4-to-connect-to-the-codec-and-make-sound/
 * @version 0.1
 * @date 2023-12-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include <stdint.h>
#include "audio.h"
#include "oscillators.h"
#include "ADSR_envelope.h"
#include "ladder_filter.h"
#include "bitcrusher.h"
#include "flanger.h"
#include "stereo_delay.h"
#include "MIDI_lut.h"
#include <stdint.h>
#include "helper_functions.h"
#include "stdlib.h"





/**
 * @brief Audio Buffer - x samples X 2 channels = 2 * x samples
 * @note Channels are interleaved - LRLRLRLRLRLRLRLRLRLRLR - audioBuffer[frame << 1] audioBuffer[(frame << 1) + 1]
 */
int16_t audioBuffer[BUFFER_SIZE]; 


extern uint8_t currentPitch;
extern uint8_t velocity, noteOn_velocity;
extern ADSR_t adsr_amp, adsr_filt;
extern ADSR_t adsr_index;
extern ZDFLadder_t Moog_filter;
extern oscillator_t osc1, osc2, osc3, osc4, sub_osc;
extern float pot1_norm, pot2_norm, pot3_norm;
extern uint16_t current_count;
extern float delay_wet, feedback;

static float f0, f1;			// frequency of oscillators 1 & 2
static float f_sub ;		// frequency of sub oscillator
static float amp_env ;		// amplitude envelope
static float filt_env ;		// filter envelope
static float index_env ;	// FM index envelope

float delayLOut = 0 ;		// left output of ping pong delay
float delayROut = 0;		// right output of ping pong delay




/**
 * @brief Initialize audio
 * 
 */
void AUDIO_Init()
{
    BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_HEADPHONE, INITIAL_VOLUME, FS);

	/**
	 * @brief Ping pong buffer
	 * @note While one audiobuffer is transmitted via DMA, another one is computed
	 * @note Hence size = 2 * (audiobuffer size)
	 */
	BSP_AUDIO_OUT_Play((uint16_t *)&audioBuffer[0], 2*BUFFER_SIZE); 

	
	
	osc_init(&osc1, 0.5, 1000, 0, 0, 0.5);
	osc_init(&osc2, 0.5, 440, 0, 1.414, 0.5);
	osc_init(&osc3, 0.5, 1000, 0, 0, 0.5);
	osc_init(&osc4, 0.5, 440, 0, 0, 0.5);
	osc_init(&sub_osc, 0.5, 440, 0, 0, 0.5);


	ADSR_init(&adsr_amp);
	ADSR_init(&adsr_filt);
	ADSR_init(&adsr_index);
	MoogLP_init(&Moog_filter);
	// flanger_init();
	Delay_init();
}

void OpSetFreq(oscillator_t * op, float f)
{
	op->freq = A0 * f;
}



/**
 * @brief Main audio function. This is where the audio samples are computed, then the audio buffer is filled
 * 
 * @param buffer 
 * @param samples 
 */
void audioBlock(int16_t *buffer, uint16_t samples)
{
	int i;
	int16_t *output;
	output = buffer;
	float sample, sampleL, sampleR;
	uint16_t valueL, valueR;
	uint8_t select_osc = current_count % 2;
	// uint8_t select_osc = clip(current_count, 0, 1);


	adsr_filt.dcy_time = 10*(0.008+pot2_norm*pot2_norm*pot2_norm*pot2_norm);
	adsr_index.dcy_time = 10*(0.008+pot2_norm*pot2_norm*pot2_norm*pot2_norm);
	adsr_filt.dcy_mult = ADSR_calculateMultiplier(1, adsr_filt.sust_level, adsr_filt.dcy_time); 
	adsr_index.dcy_mult = ADSR_calculateMultiplier(1, adsr_index.sust_level, adsr_index.dcy_time); 

	for (i = 0; i < samples; i++)
	{

		f0 = mtof[currentPitch];
		f1 = mtof[currentPitch + 7];
		
		/* test with 3 oscillators*/
		OpSetFreq(&osc1, f0);
		OpSetFreq(&osc2, f0+1);
		OpSetFreq(&osc3, f0-1);
		OpSetFreq(&osc4, f1);
		OpSetFreq(&sub_osc, f_sub);
		
		switch(select_osc){
			case 0:
			// analog waveforms
				f_sub = mtof[max(currentPitch - 12, 0)];
				
				sample = 0.25*(osc_polyblepSaw(&osc1) + osc_polyblepSaw(&osc2) + osc_polyblepSaw(&osc3) + osc_polyblepSaw(&osc4)+ osc_polyblepRect(&sub_osc));
		       
        		break;

    		case 1:
			// FM
				index_env = ADSR_compute(&adsr_index);
				osc1.FM_index = pot1_norm * pot1_norm * pot1_norm * pot1_norm * index_env;	
				sample = 0.8 * osc_FM2OP(f0);
				break;
		}

				
		// sample = osc_Sine(&osc1);

	
		
		/****************** Apply filter ***********************/
		filt_env = ADSR_compute(&adsr_filt);
		Moog_filter.cutoff =clip(7000.f * filt_env + NYQUIST * pot1_norm * pot1_norm * pot1_norm * pot1_norm, 20, NYQUIST); 
		// Moog_filter.k = 4.5f * pot2_norm;

		sample = MoogLP_compute(&Moog_filter, sample);

		/************** Apply amplitude envelope ****************/
		amp_env = ADSR_compute(&adsr_amp);
		sample *= amp_env;

		
		/************** Apply bitcrushing effect ***************/
		// sample = decimator(sample, 0.2, 16);

		/************** Apply flanging effect ***************/
		// sample = flanger_compute(sample); // TODO

		/************** Apply delay effect ****************/
		delay_wet = pot3_norm;
		feedback = clip(pot3_norm + 0.25, 0, 1.1);

		pingpongDelay_compute(sample, &delayLOut, &delayROut);
		sampleL = delayLOut;
		sampleR = delayROut;
		//sampleL = sample;
		//sampleR = sample;

		/****************** softclip outputs ********************/
		sampleL = SoftClip(sampleL);
		sampleR = SoftClip(sampleR);

		/*************** Convert to 16 bits int *****************/
		valueL = ((int16_t) ((32767.0f) * sampleL)); 
		valueR = ((int16_t) ((32767.0f) * sampleR));

		/*************** Output to audio buffer *****************/
		output[i<<1] = valueL ;			// LEFT channel
	    output[(i<<1) + 1] = valueR;	// RIGHT channel
	}
}



void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
{
	audioBlock((uint16_t *)audioBuffer, BUFFER_SIZE_DIV_4);
}

void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
{
	audioBlock((uint16_t *)(audioBuffer + BUFFER_SIZE_DIV_2), BUFFER_SIZE_DIV_4);
}

void BSP_AUDIO_OUT_Error_CallBack(void)
{
	/* Stop the program with an infinite loop */
	while (1)
	{
	}
}


void I2S3_IRQHandler(void)
{
	HAL_DMA_IRQHandler(hAudioOutI2s.hdmatx);
}
void I2S2_IRQHandler(void)
{
	HAL_DMA_IRQHandler(hAudioInI2s.hdmarx);
}