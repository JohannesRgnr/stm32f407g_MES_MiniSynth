
#include "helper_functions.h"
#include "filters.h"
#include "CONSTS.h"
#include "ladder_filter.h"
#include "lut_tan.h"


ZDFLP_t lp_L _CCM_;
ZDFLP_t lp_R _CCM_;

void SVF_LP_init(ZDFLP_t * filter){
    filter->s1 = 0.0f;
    filter->s2 = 0.0f;
    filter->r = 0.6f;           // slight resonance
    filter->cutoff = 1600;
    filter->g = 0.1;
}

float freq_to_g(float freq) // calculate g coefficient for SVF
{
	return interp_lin_lut(LUT_TAN_SIZE * (PI* freq * TS), LUT_TAN_SIZE, tan_lut);
    //return 0;
}

/* Zero-delay feedback SVF in LP mode*/
float SVF_LP_compute(ZDFLP_t *f, float sample)
{
	float bp, lp, hp;
    // float g = freq_to_g(f->cutoff);
    hp = (sample - 2.0f * f->r * f->s1 - f->g * f->s1 - f->s2) / (1.0f + 2.0f * f->r * f->g + f->g * f->g);
    bp = f->g * hp + f->s1;
	f->s1 = f->g * hp + bp; // state in 1st trapezoidal integrator
	lp = f->g * bp + f->s2;
	f->s2 = f->g * bp + lp; // state in 2nd trapezoidal integrator
	return lp;
}