/**
 * @file oscillators.c
 * @author johannes regnier
 * @brief Various oscillators
 *  General architecture and FM2OP from Xavier Halgand's Dekrispator
 *  2-sample polyblep oscillators from:
 *  Välimäki, V., Pekonen, J., & Nam, J. (2012). Perceptually informed synthesis of bandlimited classical
 *  waveforms using integrated polynomial interpolation. 
 * @version 0.1
 * @date 2023-12-26
 *
 * @copyright Copyright (c) 2023
 *
 */

// #include <math.h>
#include "oscillators.h"
#include "CONSTS.h"
#include "lut_sine.h"
#include "helper_functions.h"

oscillator_t osc1 _CCM_;
oscillator_t osc2 _CCM_;
oscillator_t sub_osc _CCM_;


void osc_init(oscillator_t *osc, float amp, float freq, float mod, float freq_mult, float pw)
{
    osc->amp = amp;
    osc->freq = A0 * freq;
    osc->phase = 0;
    osc->mod = mod;
    osc->freq_mult = freq_mult;
    osc->pw = pw;
    osc->output = 0;
}

float osc_Sine(oscillator_t *osc)
{

    osc->phase = wrap(osc->phase, 1);  // keep phase between 0 and 1
    osc->output = osc->amp * interp_lin_lut(LUT_SINE_SIZE * (osc->phase), LUT_SINE_SIZE, lut_sine); // linear-interpolated sinewave
    osc->phase += A0 * TS * osc->freq;  // increment phase (phase normalized from 0 to 1)

    return osc->output;
}

float osc_FM2OP(float freq)
{
    osc1.freq = A0 * freq;
    osc2.freq = osc2.freq_mult * A0 * freq;
    osc_Sine(&osc2);
    osc1.phase += osc1.mod * osc2.output;
    osc_Sine(&osc1);

    return osc1.output;
}


float osc_polyblepSaw(oscillator_t *osc)
{
    float dt = A0 * TS * osc->freq; // phase increment
    float t = osc->phase;   // phase
    float naiveSaw = (2.f * t) - 1.f;  // naive sawtooth (bipolar ramp)

    osc->phase += dt;       
    osc->phase = wrap(osc->phase, 1); // keep phase between 0 and 1

    // osc->output = osc->amp * naiveSaw;                       // naive sawtooth
    osc->output = osc->amp * (naiveSaw - polyblep(t, dt));      // quasi-bandlimited sawtooth
    return osc->output;
}

float osc_polyblepRect(oscillator_t * osc){
    float dt = A0 * TS * osc->freq; // phase increment
    float t = osc->phase;   // phase
    float pw = osc->pw;     // pulse width (duty cycle)
    float Rect = (t >= pw) ? -1.f : 1.f; // naive rectangle

    osc->phase += dt;       
    osc->phase = wrap(osc->phase, 1); // keep phase between 0 and 1

    Rect += polyblep(t, dt);
    Rect -= polyblep(wrap(t - pw, 1.f), dt);

    osc->output = osc->amp * Rect;
    return osc->output;
}


