/**
 * @file audio.c
 * @author johannes regnier
 * @brief Functions to make sound! 
 *  Most of it from Tom Erbe:
 *  http://synthnotes.ucsd.edu/wp4/index.php/2019/09/24/setting-up-the-stmf4-to-connect-to-the-codec-and-make-sound/
 * @version 0.1
 * @date 2023-12-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include<stdint.h>
#include "audio.h"
#include "oscillators.h"




int16_t audioBuffer[BUFFER_SIZE_DIV_2]; // 32 samples X 2 channels
float outputBuffer[BUFFER_SIZE_DIV_4];



extern oscillator_t osc1, osc2, osc3, osc4, osc5, osc6, osc7;

/**
 * @brief Initialize audio
 * 
 */
void AUDIO_Init()
{
    BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_HEADPHONE, INITIAL_VOLUME, FS);
	BSP_AUDIO_OUT_Play((uint16_t *)audioBuffer, BUFFER_SIZE);

	
	
	osc_init(&osc1, 0.5, 1000, 0, 0, 0.5);
	osc_init(&osc2, 0.5, 440, 0, 0, 0.5);
	osc_init(&osc3, 0.5, 440, 0, 0, 0.5);
	osc_init(&osc4, 0.5, 440, 0, 0, 0.5);
	osc_init(&osc5, 0.5, 440, 0, 0, 0.5);
	osc_init(&osc6, 0.5, 440, 0, 0, 0.5);
	osc_init(&osc7, 0.5, 440, 0, 0, 0.5);
	
	
}

float superSaw_outL, superSaw_outR;


void audioBlock(float *output, int32_t samples)
{
	//osc1.freq = 440.f;
	//osc2.freq = 110;

	
	for (uint16_t i = 0; i < samples; i++)
	{
		//osc_polyblepSaw(&osc1);
		//osc_polyblepSaw(&osc2);
		//osc_Sine(&osc1);
		osc_Sine(&osc1);
		// osc_superSaw(50, 0.3, &superSaw_outL, &superSaw_outR);
		// output[i << 1] 			= superSaw_outL;		// LEFT
		// output[(i << 1) + 1] 	= superSaw_outR;  		// RIGHT

		output[i << 1] 			= osc1.output;		// LEFT
		output[(i << 1) + 1] 	= osc1.output;  		// RIGHT

		
	}
}



void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
{
	audioBlock(outputBuffer, 16);
	for (int i = 0; i < 32; i += 2)
	{
		audioBuffer[i + 0] = (int16_t)((outputBuffer[i]) * 32767.0f);
		audioBuffer[i + 1] = (int16_t)((outputBuffer[i + 1]) * 32767.0f);
	}
}

void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
{
	BSP_AUDIO_OUT_ChangeBuffer((uint16_t *)audioBuffer, 64);
	audioBlock(outputBuffer, 16);
	for (int i = 0; i < 32; i += 2)
	{
		audioBuffer[i + 32] = (int16_t)((outputBuffer[i]) * 32767.0f);
		audioBuffer[i + 33] = (int16_t)((outputBuffer[i + 1]) * 32767.0f);
	}
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