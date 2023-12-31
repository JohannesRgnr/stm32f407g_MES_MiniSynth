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
// float outputBuffer[BUFFER_SIZE_DIV_4];

extern int8_t currentPitch;
extern int8_t velocity;
extern ADSR_t adsr;
extern ZDFLadder_t Moog_filter;


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
	// osc_init(&osc4, 0.5, 440, 0, 0, 0.5);
	// osc_init(&osc5, 0.5, 440, 0, 0, 0.5);
	// osc_init(&osc6, 0.5, 440, 0, 0, 0.5);
	// osc_init(&osc7, 0.5, 440, 0, 0, 0.5);

	ADSR_init(&adsr);
	MoogLP_init(&Moog_filter);
	// ADSR_setReleaseTime(&adsr, 0.4);
	// ADSR_setTarget(&adsr, 1);
}


void
OpSetFreq(oscillator_t * op, float f)
{
	op->freq = f;
}

float superSaw_outL, superSaw_outR;
static float f0 _CCM_;
static float f_sub _CCM_;
// static float vol _CCM_;
static float env _CCM_;

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

		/*--- compute ADSR envelope --*/
		env = ADSR_compute(&adsr);

		f0 = mtof[currentPitch];
		

		//osc_superSaw(frequency, 0.3, &superSaw_outL, &superSaw_outR);
		// Moog_filter.cutoff = 2000;
		OpSetFreq(&osc1, f0);
		OpSetFreq(&osc2, f0 + 0.1);
		f_sub = mtof[max(currentPitch - 12, 0)];
		OpSetFreq(&osc3, f_sub);
		// yL = 0.1*osc1.output + 0.1*osc2.output + 0.1*osc3.output;
		// yL = clip(osc1.output + osc2.output + osc3.output, -1, 1) ;
		yL = 0.2*osc_polyblepSaw(&osc1) + 0.2*osc_polyblepSaw(&osc2) + 0.2*osc_polyblepRect(&osc3);
		// yL = 0.2*osc_polyblepSaw(&osc3);
		yR = yL;
		yL *= env;

		// yL = 0.1*superSaw_outL * env;
		// yR = 0.1*superSaw_outR * env;

		yL = MoogLP_compute(&Moog_filter, yL);

		valueL = (uint16_t) ((int16_t) ((32767.0f) * yL)); 
		valueR = (uint16_t) ((int16_t) ((32767.0f) * yL));
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