/**
 * @file CONSTS.h
 * @author johannes regnier
 * @brief Constants
 * @version 0.1
 * @date 2023-12-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _CONSTS_H
#define _CONSTS_H


#define FS                  48000     // selected samplerate
/*  Actual samplerate is actually 48095 Hz (derived from I2S clock).. 
*   we take that into account for accurate tuning */
#define correctedFS         48095.f   // real samplerate (check STM32CubeMX configuration)
#define A0                  FS/correctedFS  // correcting factor for tuning
#define TS		            (1.f/FS)    // sampling period
#define BUFFER_SIZE         512         // size of the audio buffer (samples)
#define BUFFER_SIZE_DIV_2   BUFFER_SIZE/2
#define BUFFER_SIZE_DIV_4   BUFFER_SIZE/4
#define INITIAL_VOLUME      65      // initial volume level

#define PI                  3.14159265359
#define TWOPI               6.28318530718

#define MIN_MIDI_NOTE       0      // C-2
#define MAX_MIDI_NOTE       127    // G-8
#define MIDI_MAX            127.f 



#endif // !_CONSTS_H



