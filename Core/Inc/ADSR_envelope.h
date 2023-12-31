/**
 * @file ADSR_envelope.c
 * @author modified by johannes regnier
 * @brief Exponential ADSR envelope generator
 * @note Modification of Perry R. Cook and Gary P. Scavone Envelope subclass
 * @note  https://ccrma.stanford.edu/software/stk/classstk_1_1ADSR.html 
 * @note Exponential curve multiplier from https://www.martin-finke.de/articles/audio-plugins-011-envelopes/ 
 * @version 0.1
 * @date 2023-12-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */



#ifndef _ADSR_ENVELOPE_H
#define _ADSR_ENVELOPE_H

/* Includes ------------------------------------------------------------------*/

#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include "CONSTS.h"

/* Exported types ------------------------------------------------------------*/
#define ADSR_MIN_LEVEL  0.0001


//! Envelope states.
enum { ATTACK, DECAY, SUSTAIN, RELEASE, OFF};

typedef struct
{
    float   value;
	float 	target;
	int 	state;
    float   atk_time;
    float   dcy_time;
    float   sust_level;
    float   rel_time;
    float   atk_mult;
    float   dcy_mult;
    float   rel_mult;
} ADSR_t ;


/* Exported functions --------------------------------------------------------*/

//! Default constructor.
void ADSR_init(ADSR_t *env);

//! Set target = 1, state = \e ADSR::ATTACK.
void ADSR_keyOn(ADSR_t *env);

//! Set target = 0, state = \e ADSR::RELEASE.
void ADSR_keyOff(ADSR_t *env);


//! Set the attack multiplier based on a time duration.
void ADSR_setAttackTime(ADSR_t *env, float time);

//! Set the decay multiplier based on a time duration.
void ADSR_setDecayTime(ADSR_t *env, float time);

//! Set the sustain level.
void ADSR_setSustainLevel(ADSR_t *env, float level);

//! Set the release multiplier based on a time duration.
void ADSR_setReleaseTime(ADSR_t *env, float time);

// Compute multiplier to achieve exponential curve between 2 values in specified time
float ADSR_calculateMultiplier(float start_level, float end_level, float time);

//! Set sustain level and attack, decay, and release time durations.
void ADSR_setAllTimes(ADSR_t *env, float aTime, float dTime, float sLevel, float rTime);

//! Set the target value.
void ADSR_setTarget(ADSR_t *env, float target);

//! Return the current envelope \e state (ATTACK, DECAY, SUSTAIN, RELEASE, DONE).
int ADSR_getState(ADSR_t *env);

//! Set to state = ADSR::SUSTAIN with current and target values of \e aValue.
void ADSR_setValue(ADSR_t *env, float value);

float ADSR_compute( ADSR_t *env );






#endif // ! _ENVELOPE_H
