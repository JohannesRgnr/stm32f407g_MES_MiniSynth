


#include "MIDI_event.h"
#include "MIDI_lut.h"
#include "oscillators.h"
#include "gpio.h"
#include "SEGGER_RTT.h"

USBH_HandleTypeDef hUSBHost; /* USB Host handle */
MIDI_ApplicationTypeDef Appli_state = MIDI_APPLICATION_IDLE;

int pitchbend;
int keyboard[128];
uint8_t runningStatus;
float mtoinc[128];
synthVoice voice[16];

extern oscillator_t osc1, osc2, osc3, osc4, osc5, osc6, osc7;
void OpSetFreq(oscillator_t *op, float f)
{
	op->freq = f;
}

void MIDI_eventInit(){
    pitchbend = 8192;
	for (int i = 0; i < 128; i++)
	{
		mtoinc[i] = mtof[i] * 0.0000208333f; // 1/48000
		keyboard[i] = -1;
	}
	for (int i = 0; i < 16; i++)
	{
		voice[i].active = INACTIVE;
		voice[i].phase = 0.0f;
		voice[i].volume = 0.0f;
	}
}


void ProcessMIDI(midi_package_t pack)
{
	int i;
	uint8_t status;

	// Status messages that start with F, for all MIDI channels
	// None of these are implemented - though we will flash an LED
	// for the MIDI clock
	status = pack.evnt0 & 0xF0;
	if (status == 0xF0)
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
			status = runningStatus = 0x00;
			break;
		case 0xF8:										// Timing Clock (24 times a quarter note)
			HAL_GPIO_TogglePin(LD5_GPIO_Port, LD5_Pin); // RED LED
			break;
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

	// MIDI running status (same status as last message) doesn't seem to work over this USB driver
	// code commented out.

	//	else if((pack.evnt0 & 0x80) == 0x00)
	//		status = runningStatus;
	else
		runningStatus = status = pack.evnt0 & 0xF0;

	switch (status)
	{
	case 0x80: // Note Off
		// turn off all voices that match the note off note
		for (i = 0; i < 16; i++)
		{
			if (voice[i].note == pack.evnt1)
			{
				voice[i].active = RELEASE;
				keyboard[voice[i].note] = -1;
				HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
			}
		}
		break;
	case 0x90:				 // Note On
		if (pack.evnt2 == 0) // velocity 0 means note off
			// turn off all voices that match the note off note
			for (i = 0; i < 16; i++)
			{
				if ((voice[i].note == pack.evnt1) && (voice[i].active != INACTIVE))
				{
					voice[i].active = RELEASE;
					keyboard[voice[i].note] = -1;
					HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
				}
			}
		else
		{
			// if this key is already on, end the associated note and turn it off
			if (keyboard[pack.evnt1] != -1)
			{
				voice[keyboard[pack.evnt1]].active = RELEASE;
				keyboard[pack.evnt1] = -1;
				HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
			}
			// find an inactive voice and assign this key to it
			for (i = 0; i < 16; i++)
			{
				if (voice[i].active == INACTIVE)
				{
					voice[i].active = ATTACK;
					voice[i].note = pack.evnt1;
					float frequency;
					frequency = mtof[voice[i].note];
					OpSetFreq(&osc1, frequency);
					voice[i].velocity = pack.evnt2;
					keyboard[pack.evnt1] = i;
					HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET);
					SEGGER_RTT_printf(0, "Note ON, pitch %u\r\n", (int)frequency);
					break;
				}
			}
		}
		break;
	case 0xA0: // Polyphonic Pressure
		break;
	case 0xB0:				// Control Change
		switch (pack.evnt1) // CC number
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
