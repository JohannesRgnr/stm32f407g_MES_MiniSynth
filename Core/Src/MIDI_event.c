/**
 * @file MIDI_event.c
 * @author modified by johannes regnier
 * @brief MIDI events processing
 * @note Makes use of the great resources from Tom Erbe
 * @note http://synthnotes.ucsd.edu/wp4/index.php/2019/09/24/adding-the-usb-otg-midi-driver-polyphonic-framework/
 * @note and of the awesome Xavier Halgand's Dekrispator
 * @version 0.1
 * @date 2023-12-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdint.h>

#include "MIDI_event.h"
#include "gpio.h"
#include "SEGGER_RTT.h"
#include "CONSTS.h"
#include "helper_functions.h"
#include "ADSR_envelope.h"

USBH_HandleTypeDef hUSBHost; /* USB Host handle */
MIDI_ApplicationTypeDef Appli_state = MIDI_APPLICATION_IDLE;

uint16_t pitchbend ;
uint8_t currentPitch ;
uint8_t velocity, noteOn_velocity;
uint8_t notes_Active[128] = {0}; // at most, 128 MIDI notes are active
int8_t notesCount = 0 ;			 // number of notes active
extern ADSR_t adsr_amp;
extern ADSR_t adsr_filt;
extern ADSR_t adsr_index;


/**
 * @brief turns off all current active notes
 * 
 */
void allNotesOff(void)
{
	for (uint8_t i = 0; i < 128; i++)
	{
		notes_Active[i] = 0;
		ADSR_keyOff(&adsr_amp); // release envelopes
		ADSR_keyOff(&adsr_filt);
		ADSR_keyOff(&adsr_index);
	}
	notesCount = 0;
}


void MIDI_eventInit()
{
	pitchbend = 8192;
}

/**
 * @brief Process incoming MIDI events - Monophonic, with legato
 * @note Code adapted from Xavier Halgand and from Tom Erbe's version
 *
 * @param pack
 */
void ProcessMIDI(midi_package_t pack)
{
	uint8_t status;

	status = pack.evnt0 & 0xF0;
	if (status == 0xF0) // not using any of these
	{
		switch (pack.evnt0)
		{
		case 0xF0: // Start of System Exclusive
		case 0xF1: // MIDI Time Code Quarter Fram
		case 0xF2: // Song Position Pointer
		case 0xF3: // Song Select
		case 0xF4: // Undefined
		case 0xF5: // Undefined
		case 0xF6: // Tune Request
		case 0xF7: // End of System Exclusive
		case 0xF8: // Timing Clock (24 times a quarter note)
		case 0xF9: // Undefined
		case 0xFA: // Start Sequence
		case 0xFB: // Continue Sequence
		case 0xFC: // Pause Sequence
		case 0xFD: // Undefined
		case 0xFE: // Active Sensing
		case 0xFF: // Reset all synthesizers to power up
			break;
		}
	}

	switch (status)
	{
	/********************* Real Note Off *************************/	
	case 0x80: 
		uint8_t noteOff = pack.evnt1;
		notes_Active[noteOff] = 0;
		notesCount--;
		if (notesCount <= 0) // no notes active anymore
		{
			ADSR_keyOff(&adsr_amp); // release envelopes
			ADSR_keyOff(&adsr_filt);
			ADSR_keyOff(&adsr_index);
			notesCount = 0;
			HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
		}
		else // legato 
		{
			if (noteOff  == currentPitch) // let sound the lowest key pressed
			{
				uint8_t i;
				for (i = 0; i < 128; i++)
				{
					if (notes_Active[i] == 1) // find the lowest key pressed
						break;
				}
				currentPitch = i; 
			}
		}
		break;
	
	case 0x90: // Note On
		uint8_t noteOn = pack.evnt1;
		velocity = pack.evnt2;
		/*********************  Note On *************************/	
		if (velocity > 0) 
		{			
			currentPitch = noteOn;
			noteOn_velocity = velocity;
			ADSR_keyOn(&adsr_amp); // start envelopes
			ADSR_keyOn(&adsr_filt);
			ADSR_keyOn(&adsr_index);
			//SEGGER_RTT_printf(0, "Note ON, pitch %u\r\n", currentPitch); // debug
			HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET); // red LED ON when incoming MIDI note on
			notesCount++;
			notes_Active[noteOn] = 1;
		}
		/*********************  Note Off *************************/
		else //  common implementation in MIDI devices: Note Off = Note On with velocity 0
		{			
			notes_Active[noteOn] = 0;
			notesCount--;
			//SEGGER_RTT_printf(0, "Note OFF, pitch %u\r\n", currentPitch); // debug
			if (notesCount <= 0)
			{
				ADSR_keyOff(&adsr_amp);		// release envelopes
				ADSR_keyOff(&adsr_filt);
				ADSR_keyOff(&adsr_index);
				notesCount = 0;
				HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET); // red LED OFF when all MIDI notes off
			}
			else // legato
			{
				if (noteOn == currentPitch)
				{
					uint8_t i;
					for (i = 0; i < 128; i++)
					{
						if (notes_Active[i] == 1) // find the lowest key pressed
							break;
					}
					currentPitch = i; 
				}
			}
		}
	/********************* Other MIDI Messages *************************/	
	case 0xA0: 	// Polyphonic Pressure
		break;
	case 0xB0:	// Control Change
		// uint8_t cc_number = pack.evnt1;
		// uint8_t cc_value = pack.evnt2;
		// SEGGER_RTT_printf(0, "CC#  %u %u\r\n", cc_number, cc_value); // debug
		switch (pack.evnt1) // TODO CC number --> implement CC actions
		{
		case 20:
			break;
		case 7:
			break; // master volume
		}
		break;
	case 0xC0: // Program Change
		break;
	case 0xD0: // After Touch
		break;
	case 0xE0: // Pitch Bend
		pitchbend = pack.evnt2 * 128 + pack.evnt1;
		break;
	}
}




/*====================================================================================================*/
/**
 * @brief  User Process function callback
 * @param  phost: Host Handle
 * @param  id: Host Library user message ID
 * @retval none
 */
void USBH_UserProcess_callback(USBH_HandleTypeDef *pHost, uint8_t vId)
{
	switch (vId)
	{
	case HOST_USER_SELECT_CONFIGURATION:
		break;

	case HOST_USER_DISCONNECTION:
		Appli_state = MIDI_APPLICATION_DISCONNECT;
		// 4 - green, 3 - orange, 5 - red, 6 - blue
		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_RESET);
		break;

	case HOST_USER_CLASS_ACTIVE:
		Appli_state = MIDI_APPLICATION_READY;
		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
		break;

	case HOST_USER_CONNECTION:
		Appli_state = MIDI_APPLICATION_START;
		HAL_GPIO_WritePin(GPIOD, 0x8000, GPIO_PIN_SET);
		break;

	default:
		break;
	}
}


/**
 * @brief User button EXTI Line0 External Interrupt ISR Handler CallBack --> MIDI panic --> all notes off
 * 
 * @param GPIO_Pin 
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	allNotesOff();
	HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET); // red LED OFF when all MIDI notes off
}
