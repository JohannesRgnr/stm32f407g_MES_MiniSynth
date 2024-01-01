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
#include "stereo_delay.h"
#include "MIDI_lut.h"
#include <stdint.h>
#include "helper_functions.h"



/**
 * @brief Audio Buffer - x samples X 2 channels = 2 * x samples
 * @note Channels are interleaved - LRLRLRLRLRLRLRLRLRLRLR - audioBuffer[frame << 1] audioBuffer[(frame << 1) + 1]
 */
int16_t audioBuffer[BUFFER_SIZE]; 


extern uint8_t currentPitch;
extern uint8_t velocity;
extern ADSR_t adsr_amp, adsr_filt;
extern ZDFLadder_t Moog_filter;
extern oscillator_t osc1, osc2, osc3;


static float f0 _CCM_;
static float f_sub _CCM_;
// static float vol _CCM_;
static float amp_env _CCM_;
static float filt_env _CCM_;


static float delayLOut _CCM_;
static float delayROut _CCM_;



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
	osc_init(&osc2, 0.5, 440, 0, 0, 0.5);
	osc_init(&osc3, 0.5, 440, 0, 0, 0.5);


	ADSR_init(&adsr_amp);
	ADSR_init(&adsr_filt);
	MoogLP_init(&Moog_filter);
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
void audioBlock(uint16_t *buffer, uint16_t samples)
{
	int i;
	int16_t *output;
	output = buffer;
	float sample, sampleL, sampleR;
	uint16_t valueL, valueR;
	

	for (i = 0; i < samples; i++)
	{

		f0 = mtof[currentPitch];
		
		/* test with 3 sawtooth*/
		OpSetFreq(&osc1, f0);
		OpSetFreq(&osc2, f0 + 0.1);

		f_sub = mtof[max(currentPitch - 12, 0)];
		OpSetFreq(&osc3, f_sub);
		sample = osc_polyblepSaw(&osc1) + osc_polyblepSaw(&osc2) + osc_polyblepRect(&osc3);
	

		/* test with supersaw */
		// OpSetFreq(&osc1, f0);
		// OpSetFreq(&osc2, f0 * (1 - 0.3 * 0.01953125));
		// OpSetFreq(&osc3, f0 * (1 + 0.3 * 0.01953125));
		// OpSetFreq(&osc4, f0 * (1 + 0.3 * 0.0625));
		// OpSetFreq(&osc5, f0 * (1 - 0.3 * 0.109375));
		// OpSetFreq(&osc6, f0 * (1 + 0.3 * 0.109375));
		// yL = 0.25 * (0.707 * osc_polyblepSaw(&osc1) + osc_polyblepSaw(&osc2) + osc_polyblepSaw(&osc3) + osc_polyblepSaw(&osc4));
    	// yR = 0.25 * (0.707 * osc_polyblepSaw(&osc1) + osc_polyblepSaw(&osc5) + osc_polyblepSaw(&osc6) );
		

		/****************** Apply filter ***********************/
		filt_env = ADSR_compute(&adsr_filt);
		Moog_filter.cutoff = 600 * filt_env;
		// Moog_filterR.cutoff = 1600 * filt_env;
		sample = MoogLP_compute(&Moog_filter, sample);
	

		/************** Apply amplitude envelope ****************/
		amp_env = ADSR_compute(&adsr_amp);
		sample *= amp_env;

		
		/************** Apply bitcrushing effect ***************/
		// sample = decimator(sample, 1., 4);

		/************** Apply delay effect ****************/
		// pingpongDelay_compute(sample, &delayLOut, &delayROut);
		pingpongDelay_compute(sample, &delayLOut, &delayROut);
		sampleL = delayLOut;
		sampleR = delayROut;

		/****************** softclip outputs ********************/
		sampleL = SoftClip(sampleL);
		sampleR = SoftClip(sampleR);

		/*************** Convert to 16 bits int *****************/
		valueL = ((int16_t) ((32767.0f) * sampleL)); 
		valueR = ((int16_t) ((32767.0f) * sampleR));

		/*************** Output to audio buffer *****************/
		output[i<<1] = valueL ;			// LEFT channel
	    output[(i<<1) + 1] = valueR;	// RIGHT channel
		// *output++ = valueL; // left channel sample
		// *output++ = valueR; // right channel sample
		
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