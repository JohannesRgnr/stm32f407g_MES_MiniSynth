#ifndef _MIDI_EVENT_H
#define _MIDI_EVENT_H

#ifndef __USBH_MIDI_CORE_H
#include "usbh_MIDI.h"
#endif
#include "MIDI_application.h"





typedef struct synthVoice
{
	int active;
	int note;
	int velocity;
	float volume;
	float frequency;
	float phase;
} synthVoice;

typedef enum
{
	INACTIVE = 0,
	ATTACK,
	DECAY,
	SUSTAIN,
	RELEASE
} voiceState;



/* USER CODE BEGIN EFP */
void USBH_UserProcess_callback(USBH_HandleTypeDef *pHost, uint8_t vId);
void MIDI_eventInit(void);
void ProcessMIDI(midi_package_t pack);
/* USER CODE END EFP */




#endif // !_MIDI_EVENT_H