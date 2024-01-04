/**
 * @file oscillators.h
 * @author johannes regnier
 * @brief Various oscillators
 * @version 0.1
 * @date 2023-12-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _OSCILLATORS_H
#define _OSCILLATORS_H

#include <stdint.h>
#include <stdbool.h>





enum osc_type
{
    sine = 0,
    saw,
    rect,
    supersaw,
    fm_2op
};

typedef struct
{
    float amp;
    float freq;
    float phase;
    float FM_index;
    float FM_ratio;
    float pw;
    float output;
} oscillator_t;


inline float polyblep(float t, float dt){
    float blep = 0.f;
    if (t < dt)
    {
        t /= dt;
        blep = t + t - t * t - 1.f;
    }
    else if (t > 1.f - dt)
    {
        t = (t - 1.f) / dt;
        blep = t * t + t + t + 1.f;
    }
    return blep;
}

void    osc_init(oscillator_t * osc, float amp, float freq, float mod, float freq_mult, float pw);
float   osc_Sine(oscillator_t * osc);
float   osc_FM2OP(float freq);
float   osc_polyblepSaw(oscillator_t * osc);
float   osc_polyblepRect(oscillator_t * osc);



#endif // !_OSCILLATORS_H
