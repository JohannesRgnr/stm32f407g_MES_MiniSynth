/*
 * MIDI_application.h
 *
 *  Created on: 6 dec. 2014
 *      Author: Xavier Halgand
 */

#ifndef MIDI_APPLICATION_H_
#define MIDI_APPLICATION_H_

/* Includes ------------------------------------------------------------------*/

#include "stdio.h"
#ifndef __USBH_CORE_
#include "usbh_core.h"
#endif
#ifndef __USBH_MIDI_CORE_H
#include "usbh_MIDI.h"
#endif

#include <math.h>
#include <stdint.h>
#include <stdbool.h>

/*------------------------------------------------------------------------------*/
typedef enum
{
	MIDI_APPLICATION_IDLE = 0,
	MIDI_APPLICATION_START,
	MIDI_APPLICATION_READY,
	MIDI_APPLICATION_RUNNING,
	MIDI_APPLICATION_DISCONNECT
}
MIDI_ApplicationTypeDef;

/*------------------------------------------------------------------------------*/


/* Exported functions ------------------------------------------------------- */

void MIDI_Application(void);

/*------------------------------------------------------------------------------*/
#endif /* MIDI_APPLICATION_H_ */
