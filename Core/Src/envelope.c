/**
 ******************************************************************************
 * File Name          	: adsr.c
 * Author				: Perry R. Cook and Gary P. Scavone, modified by Xavier Halgand
 * Date               	:
 * Description        	:
 ******************************************************************************
 */

/***************************************************/
/*
    This Envelope subclass implements a
    traditional ADSR (Attack, Decay,
    Sustain, Release) envelope.  It
    responds to simple keyOn and keyOff
    messages, keeping track of its state.
    The \e state = ADSR::DONE after the
    envelope value reaches 0.0 in the
    ADSR::RELEASE state.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2005.
*/
/*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
*/
/***************************************************/

#include "envelope.h"
// #include "SEGGER_RTT.h"
#include <stdint.h>
#include "lut_log10.h"
#include "helper_functions.h"

/*---------------------------------------------------------------------------*/

ADSR_t			adsr;
float multiplier[3] = {0};
/*---------------------------------------------------------------------------*/



void ADSR_init(ADSR_t *env)
{
    env->target_ = 1.0;
    env->value_ = 0.0;
    env->state_ = OFF;
    env->atk_time = 0.01;
    env->dcy_time = 0.1;
    env->sust_level = 0.8;
    env->rel_time = 1; 
    env->atk_mult = ADSR_calculateMultiplier(ADSR_MIN_LEVEL, 1.0, env->atk_time);; 
    env->dcy_mult = ADSR_calculateMultiplier(1.0, env->sust_level, env->dcy_time); ;
    env->rel_mult = ADSR_calculateMultiplier(env->sust_level, ADSR_MIN_LEVEL, env->rel_time);;
}

void ADSR_keyOn(ADSR_t *env)
{
	env->target_ = 1.0f;
    env->value_ = ADSR_MIN_LEVEL;
    env->state_ = ATTACK;
}

void ADSR_keyOff(ADSR_t *env)
{
	env->target_ = 0.0;
	env->state_ = RELEASE;
}

// void ADSR_setAttackRate(ADSR_t *env, float rate)
// {
// 	env->attackRate_ = rate;
// }

// void ADSR_setDecayRate(ADSR_t *env, float rate)
// {
// 	env->decayRate_ = rate;
// }

void ADSR_setSustainLevel(ADSR_t *env, float level)
{
	env->sust_level = level;
}

// void ADSR_setReleaseRate(ADSR_t *env, float rate)
// {
// 	env->releaseRate_ = rate;
    
// }

// void ADSR_setAttackTime(ADSR_t *env, float time)
// {
// 	env->attackRate_ = 1.0 / ( time * FS );
// }

// void ADSR_setDecayTime(ADSR_t *env, float time)
// {
// 	env->decayRate_ = 1.0 / ( time * FS );
// }

// void ADSR_setReleaseTime(ADSR_t *env, float time)
// {
// 	env->releaseRate_ = env->sustainLevel_ / ( time * FS );
//     env->release_mult = 1.0 + (interp_lin_lut(LUT_LOG10_SIZE * (0.001), LUT_LOG10_SIZE, log10_lut) - interp_lin_lut(LUT_LOG10_SIZE * (env->sustainLevel_), LUT_LOG10_SIZE, log10_lut)) / (time * FS);
// }

float ADSR_calculateMultiplier(float start_level, float end_level, float time){
    return  1.0 + (interp_lin_lut(LUT_LOG10_SIZE * (end_level), LUT_LOG10_SIZE, log10_lut) - interp_lin_lut(LUT_LOG10_SIZE * (start_level), LUT_LOG10_SIZE, log10_lut)) / (time * FS);
}

// void ADSR_setAllTimes(ADSR_t *env, float aTime, float dTime, float sLevel, float rTime)
// {
//   ADSR_setAttackTime(env, aTime);
//   ADSR_setDecayTime(env, dTime);
//   ADSR_setSustainLevel(env, sLevel);
//   ADSR_setReleaseTime(env, rTime);
// }

// void ADSR_setTarget(ADSR_t *env, float target)
// {
// 	env->target_ = target;
//   if (env->value_ < env->target_) {
// 	  env->state_ = ATTACK;
// 	  ADSR_setSustainLevel(env, env->target_);
//     env->rate_ = env->attackRate_;
//   }
//   if (env->value_ > env->target_) {
// 	  ADSR_setSustainLevel(env, env->target_);
//     env->state_ = DECAY;
//     env->rate_ = env->decayRate_;
//   }
// }

// void ADSR_setValue(ADSR_t *env, float value)
// {
// 	env->state_ = SUSTAIN;
// 	env->target_ = value;
// 	env->value_ = value;
// 	ADSR_setSustainLevel(env, value);
// 	env->rate_ = 0.0f;
// }

// int ADSR_getState(ADSR_t *env)
// {
//   return env->state_;
// }

// void AttTime_set(uint8_t val)
// {
// 	ADSR_setAttackTime(&adsr, val/MIDI_MAX + 0.0001f);
// }
// void DecTime_set(uint8_t val)
// {
// 	ADSR_setDecayTime(&adsr, .2*val/MIDI_MAX + 0.0001f);
// }
// void SustLevel_set(uint8_t val)
// {
// 	ADSR_setSustainLevel(&adsr, val/MIDI_MAX);
// }
// void RelTime_set(uint8_t val)
// {
// 	ADSR_setReleaseTime(&adsr, .5f * val/MIDI_MAX + 0.0001f);
// }
/*--------------------------------------------------------------------------------------*/




float ADSR_computeSample(ADSR_t *env)
{
    switch (env->state_)
    {

    case ATTACK:
        env->value_ *= env->atk_mult;
        if (env->value_ >= env->target_)
        {
            env->value_ = env->target_;
            env->target_ = env->sust_level;
            env->state_ = DECAY;
        }
        break;

    case DECAY:
        env->value_ *= env->dcy_mult;
        if (env->value_ <= env->sust_level)
        {
            env->value_ = env->sust_level;
            env->state_ = SUSTAIN;
        }
        break;

    case RELEASE:
        env->value_ *= env->rel_mult;
        if (env->value_ <= ADSR_MIN_LEVEL)
        {
            env->value_ = 0.0f;
            env->state_ = OFF;
        }
    }


  return env->value_;
}

/*******************************************************   EOF   ***********************************************************/