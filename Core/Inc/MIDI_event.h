/**
 * @file MIDI_event.h
 * @author modified by johannes regnier
 * @brief MIDI events processing
 * @version 0.1
 * @date 2023-12-30
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#ifndef _MIDI_EVENT_H
#define _MIDI_EVENT_H

#ifndef __USBH_MIDI_CORE_H
#include "usbh_MIDI.h"
#endif
#include "MIDI_application.h"





void allNotesOff(void);

void USBH_UserProcess_callback(USBH_HandleTypeDef *pHost, uint8_t vId);
void MIDI_eventInit(void);
void ProcessMIDI(midi_package_t pack);





#endif // !_MIDI_EVENT_H