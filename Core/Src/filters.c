
/**
 * @file filters.c
 * @author johannes regnier
 * @brief various filters
 * @version 0.1
 * @date 2024-01-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "helper_functions.h"
#include "filters.h"
#include "CONSTS.h"
#include "ladder_filter.h"
#include "lut_tan.h"


ZDFLP_t lp_L ;
ZDFLP_t lp_R ;

onepoleLP_t smooth_ADC1;
onepoleLP_t smooth_ADC2;
onepoleLP_t smooth_ADC3;



void SVF_LP_init(ZDFLP_t * filter){
    filter->s1 = 0.0f;
    filter->s2 = 0.0f;
    filter->r = 0.65f;           // slight resonance
    filter->cutoff = 1200;    
}

/**
 * @brief calculate g coefficient for SVF
 * 
 * @param freq 
 * @return float 
 */
float freq_to_g(float freq) 
{
	return interp_lin_lut(LUT_TAN_SIZE * (PI* freq * TS), LUT_TAN_SIZE, tan_lut);
    //return 0;
}

/**
 * @brief 2-pole resonant state variable filter, in lowpass mode
 * 
 * @param f 
 * @param sample 
 * @return float 
 */
float SVF_LP_compute(ZDFLP_t *f, float sample)
{
	float bp, lp, hp;
    f->g = freq_to_g(f->cutoff);
    hp = (sample - 2.0f * f->r * f->s1 - f->g * f->s1 - f->s2) / (1.0f + 2.0f * f->r * f->g + f->g * f->g);
    bp = f->g * hp + f->s1;
	f->s1 = f->g * hp + bp; // state in 1st trapezoidal integrator
	lp = f->g * bp + f->s2;
	f->s2 = f->g * bp + lp; // state in 2nd trapezoidal integrator
	return lp;
}


/**
 * @brief A simple lowpass filter, useful to smooth data. 
 * @note    alpha = 0: no filtering. alpha close to 1: strong smoothing/filtering
 * @param f 
 * @param sample 
 * @return float 
 */
float smoothing_LP(onepoleLP_t *f, float sample, float alpha){
  float output;
  output = alpha * f->old_value + (1 - alpha) * sample;
  f->old_value = output;
  return output;
}