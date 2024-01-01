/**
 * @file ADSR_envelope.c
 * @author modified by johannes regnier
 * @brief Exponential ADSR envelope generator
 * @note Modification of Perry R. Cook and Gary P. Scavone Envelope subclass (which has linear segments)
 * @note  https://ccrma.stanford.edu/software/stk/classstk_1_1ADSR.html 
 * @note Exponential curve multiplier from https://www.martin-finke.de/articles/audio-plugins-011-envelopes/ 
 * @version 0.1
 * @date 2023-12-31
 * 
 * @copyright Copyright (c) 2023
 * 
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

#include "ADSR_envelope.h"
#include <stdint.h>
#include "lut_log.h"
#include "helper_functions.h"

/*---------------------------------------------------------------------------*/

ADSR_t			adsr_amp _CCM_;
ADSR_t			adsr_filt _CCM_;

/*---------------------------------------------------------------------------*/



void ADSR_init(ADSR_t *env)
{
    env->target = 1.0;
    env->value = 0.0;
    env->state = OFF;
    env->atk_time = 0.005;
    env->dcy_time = 5;
    env->sust_level = 0.1;
    env->rel_time = 2; 
    env->atk_mult = ADSR_calculateMultiplier(ADSR_MIN_LEVEL, env->target, env->atk_time); 
    env->dcy_mult = ADSR_calculateMultiplier(env->target, env->sust_level, env->dcy_time); 
    env->rel_mult = ADSR_calculateMultiplier(env->sust_level, ADSR_MIN_LEVEL, env->rel_time);
}

void ADSR_keyOn(ADSR_t *env)
{
	env->target = 1.0f;
    env->value = ADSR_MIN_LEVEL; // start at very low value != 0, if not, multiplier does nothing
    env->state = ATTACK;
}

void ADSR_keyOff(ADSR_t *env)
{
	env->target = 0.0;
	env->state = RELEASE;
}





void ADSR_setAttackTime(ADSR_t *env, float time)
{
    env->atk_mult = ADSR_calculateMultiplier(ADSR_MIN_LEVEL, env->target, env->atk_time);
}

void ADSR_setDecayTime(ADSR_t *env, float time)
{
	env->dcy_mult = ADSR_calculateMultiplier(env->target, env->sust_level, env->dcy_time); 
}

void ADSR_setSustainLevel(ADSR_t *env, float level)
{
	env->sust_level = level;
}

void ADSR_setReleaseTime(ADSR_t *env, float time)
{
    env->rel_mult = ADSR_calculateMultiplier(env->sust_level, ADSR_MIN_LEVEL, env->rel_time);
}

/**
 * @brief Compute multiplier to achieve exponential curve between 2 values in specified time
 * @note uses lookup table for log instead of math log function
 * @note formula comes from https://www.martin-finke.de/articles/audio-plugins-011-envelopes/  
 * @param start_level 
 * @param end_level 
 * @param time
 * @return float 
 */
float ADSR_calculateMultiplier(float start_level, float end_level, float time){
    return  1.0 + (interp_lin_lut(LUT_LOG_SIZE * (end_level), LUT_LOG_SIZE, log_lut) - interp_lin_lut(LUT_LOG_SIZE * (start_level), LUT_LOG_SIZE, log_lut)) / (time * FS);
}

void ADSR_setAllTimes(ADSR_t *env, float aTime, float dTime, float sLevel, float rTime)
{
  ADSR_setAttackTime(env, aTime);
  ADSR_setDecayTime(env, dTime);
  ADSR_setSustainLevel(env, sLevel);
  ADSR_setReleaseTime(env, rTime);
}

void ADSR_setTarget(ADSR_t *env, float target)
{
	env->target = target;
  if (env->value < env->target) {
	  env->state = ATTACK;
	  ADSR_setSustainLevel(env, env->target);
    
  }
  if (env->value > env->target) {
	  ADSR_setSustainLevel(env, env->target);
    env->state = DECAY;
  }
}

void ADSR_setValue(ADSR_t *env, float value)
{
	env->state = SUSTAIN;
	env->target = value;
	env->value = value;
	ADSR_setSustainLevel(env, value);
}

int ADSR_getState(ADSR_t *env)
{
  return env->state;
}

/*--------------------------------------------------------------------------------------*/




float ADSR_compute(ADSR_t *env)
{
    switch (env->state)
    {

    case ATTACK:
        env->value *= env->atk_mult;
        if (env->value >= env->target)
        {
            env->value = env->target;
            env->target = env->sust_level;
            env->state = DECAY;
        }
        break;

    case DECAY:
        env->value *= env->dcy_mult;
        if (env->value <= env->sust_level)
        {
            env->value = env->sust_level;
            env->state = SUSTAIN;
        }
        break;

    case RELEASE:
        env->value *= env->rel_mult;
        if (env->value <= ADSR_MIN_LEVEL)
        {
            env->value = 0.0f;
            env->state = OFF;
        }
    }
  return env->value;
}

