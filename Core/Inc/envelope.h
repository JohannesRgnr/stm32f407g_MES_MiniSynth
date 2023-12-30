/**
 * @file envelope.h
 * @author modified by johannes regnier
 * @brief ADSR envelope generator
 * @note modification of Xavier Halgand version, which was itself an implementation of the STK ADSR envelope class
 * @note by Perry R. Cook and Gary P. Scavone https://ccrma.stanford.edu/software/stk/classstk_1_1ADSR.html 
 * @version 0.1
 * @date 2023-12-30
 * 
 * @copyright Copyright (c) 2023
 * 
 */



#ifndef _ENVELOPE_H
#define _ENVELOPE_H

/* Includes ------------------------------------------------------------------*/

#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include "CONSTS.h"

/* Exported types ------------------------------------------------------------*/
#define ADSR_MIN_LEVEL  0.0001


//! Envelope states.
enum { OFF, ATTACK, DECAY, SUSTAIN, RELEASE, kNumEnvelopeStages };

typedef struct
{
	float 	attackRate_;
	float 	decayRate_;
    float   decay_mult;
    float   sustainLevel_;
    float 	releaseRate_;
    float   release_mult;
    float   value_;
    float	lastOutput_;
	float 	target_;
	float	rate_;
	int 	state_;
    uint32_t sample_index;
    uint32_t nextStageSampleIndex;
    float   stageValue[kNumEnvelopeStages];
    uint32_t cnt_; // sample counter for on state

} ADSR_t ;


/* Exported functions --------------------------------------------------------*/

//! Default constructor.
void ADSR_init(ADSR_t *env);

//! Set target = 1, state = \e ADSR::ATTACK.
void ADSR_keyOn(ADSR_t *env);

//! Set target = 0, state = \e ADSR::RELEASE.
void ADSR_keyOff(ADSR_t *env);

//! Set the attack rate.
void ADSR_setAttackRate(ADSR_t *env, float rate);

//! Set the decay rate.
void ADSR_setDecayRate(ADSR_t *env, float rate);

//! Set the sustain level.
void ADSR_setSustainLevel(ADSR_t *env, float level);

//! Set the release rate.
void ADSR_setReleaseRate(ADSR_t *env, float rate);

//! Set the attack rate based on a time duration.
void ADSR_setAttackTime(ADSR_t *env, float time);

//! Set the decay rate based on a time duration.
void ADSR_setDecayTime(ADSR_t *env, float time);

//! Set the release rate based on a time duration.
void ADSR_setReleaseTime(ADSR_t *env, float time);

//! Set sustain level and attack, decay, and release time durations.
void ADSR_setAllTimes(ADSR_t *env, float aTime, float dTime, float sLevel, float rTime);

//! Set the target value.
void ADSR_setTarget(ADSR_t *env, float target);

//! Return the current envelope \e state (ATTACK, DECAY, SUSTAIN, RELEASE, DONE).
int ADSR_getState(ADSR_t *env) ;

//! Set to state = ADSR::SUSTAIN with current and target values of \e aValue.
void ADSR_setValue(ADSR_t *env, float value);

float ADSR_computeSample( ADSR_t *env );

void setGateTime(uint8_t val);
void AttTime_set(uint8_t val);
void DecTime_set(uint8_t val);
void SustLevel_set(uint8_t val);
void RelTime_set(uint8_t val);



#endif // ! _ENVELOPE_H
