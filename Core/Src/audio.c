/**
 * @file audio.c
 * @author johannes regnier
 * @brief Functions to make sound! 
 * @note  Most of it from Tom Erbe:
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
#include "filter.h"
#include "MIDI_lut.h"
#include <stdint.h>
#include "helper_functions.h"




uint16_t audioBuffer[BUFFER_SIZE]; // 256 samples X 2 channels = 512 samples


extern int8_t currentPitch;
extern int8_t velocity;
extern ADSR_t adsr_amp, adsr_filt;
extern ZDFLadder_t Moog_filterL, Moog_filterR;
extern oscillator_t osc1, osc2, osc3, osc4, osc5, osc6, osc7;

/**
 * @brief Initialize audio
 * 
 */
void AUDIO_Init()
{
    BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_HEADPHONE, INITIAL_VOLUME, FS);
	BSP_AUDIO_OUT_Play((uint16_t *)&audioBuffer[0], 2*BUFFER_SIZE); // 1024 samples

	
	
	osc_init(&osc1, 0.5, 1000, 0, 0, 0.5);
	osc_init(&osc2, 0.5, 440, 0, 0, 0.5);
	osc_init(&osc3, 0.5, 440, 0, 0, 0.5);
	osc_init(&osc4, 0.5, 440, 0, 0, 0.5);
	osc_init(&osc5, 0.5, 440, 0, 0, 0.5);
	osc_init(&osc6, 0.5, 440, 0, 0, 0.5);
	osc_init(&osc7, 0.5, 440, 0, 0, 0.5);

	ADSR_init(&adsr_amp);
	ADSR_init(&adsr_filt);
	MoogLP_init(&Moog_filterL);
	MoogLP_init(&Moog_filterR);

}


void
OpSetFreq(oscillator_t * op, float f)
{
	op->freq = A0 * f;
}


static float f0 _CCM_;
static float f_sub _CCM_;
// static float vol _CCM_;
static float amp_env _CCM_;
static float filt_env _CCM_;
static float ssawL _CCM_;
static float ssawR _CCM_;

void audioBlock(uint16_t *buffer, uint16_t samples)
{
	//osc1.freq = 440.f;
	//osc2.freq = 110;
	int i;
	uint16_t *output;
	output = buffer;
	float yL, yR;
	uint16_t valueL, valueR;
	

	for (i = 0; i < samples; i++)
	{
		
		// osc_Sine(&osc1);

		

		f0 = mtof[currentPitch];
		

		
		/* test with 3 sawtooth*/
		// OpSetFreq(&osc1, f0);
		// OpSetFreq(&osc2, f0 + 0.1);

		// OpSetFreq(&osc4, f0 - 0.2);
		// OpSetFreq(&osc5, f0 + 1);
		// OpSetFreq(&osc6, f0 + 5);
		// f_sub = mtof[max(currentPitch - 12, 0)];
		// OpSetFreq(&osc3, f_sub);
		// yL = osc_polyblepSaw(&osc1) + osc_polyblepSaw(&osc2) + osc_polyblepRect(&osc3) + osc_polyblepSaw(&osc4) + osc_polyblepSaw(&osc5) + osc_polyblepRect(&osc6);
	

		/* test with supersaw */
		OpSetFreq(&osc1, f0);
		OpSetFreq(&osc2, f0 * (1 - 0.3 * 0.01953125));
		OpSetFreq(&osc3, f0 * (1 + 0.3 * 0.01953125));
		OpSetFreq(&osc4, f0 * (1 + 0.3 * 0.0625));
		OpSetFreq(&osc5, f0 * (1 - 0.3 * 0.109375));
		OpSetFreq(&osc6, f0 * (1 + 0.3 * 0.109375));
		yL = 0.25 * (0.707 * osc_polyblepSaw(&osc1) + osc_polyblepSaw(&osc2) + osc_polyblepSaw(&osc3) + osc_polyblepSaw(&osc4));
    	yR = 0.25 * (0.707 * osc_polyblepSaw(&osc1) + osc_polyblepSaw(&osc5) + osc_polyblepSaw(&osc6) );
		// OpSetFreq(&osc6, f0 + 5);

		/*--- compute ADSR amplitude envelope --*/
		amp_env = ADSR_compute(&adsr_amp);
		yL *= amp_env;
		yR *= amp_env;
		
		/*--- compute ADSR filter envelope --*/
		filt_env = ADSR_compute(&adsr_filt);
		Moog_filterL.cutoff = 600 * filt_env;
		Moog_filterR.cutoff = 600 * filt_env;
		yL = MoogLP_compute(&Moog_filterL, yL);
		yR = MoogLP_compute(&Moog_filterR, yR);
		
		/* clip between - 1 and 1 */
		yL = SoftClip(yL);
		yR = SoftClip(yR);

		valueL = (uint16_t) ((int16_t) ((32767.0f) * yL)); 
		valueR = (uint16_t) ((int16_t) ((32767.0f) * yR));
		*output++ = valueL; // left channel sample
		*output++ = valueR; // right channel sample
		
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