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


// #define FS                  48000     // selected samplerate
// /*  Actual samplerate is actually 48095 Hz (derived from I2S clock).. 
// *   we take that into account for accurate tuning */
// #define correctedFS         48095.f   // real samplerate (check STM32CubeMX configuration)

#define FS                  44100     // selected samplerate
/*  Actual samplerate is actually 44020 Hz (derived from I2S clock).. 
*   we take that into account for accurate tuning */
#define correctedFS         44020.f   // real samplerate (check STM32CubeMX configuration)



#define A0                  FS/correctedFS  // correcting factor for tuning
#define TS		            (1.f/FS)        // sampling period
#define NYQUIST		        FS/2.f          // Nyquist frequency
#define BUFFER_SIZE         256             // size of the audio buffer (samples)
#define BUFFER_SIZE_DIV_2   (BUFFER_SIZE/2)
#define BUFFER_SIZE_DIV_4   (BUFFER_SIZE/4)
#define INITIAL_VOLUME      85              // initial volume level



#define PI                  3.14159265359
#define TWOPI               6.28318530718

#define MIDI_MAX            127.f 
#define MIDI_MAXi           127

#define _CCM_					__attribute__((section(".ccmram"))) // for use of CCM RAM (64kB)

#if defined   (__GNUC__)        /* GNU Compiler */
    #define __ALIGN    __attribute__ ((aligned (4)))
#endif /* __GNUC__ */


#endif // !_CONSTS_H



