

#include "filter.h"
#include "helper_functions.h"
#include "CONSTS.h"
#include "lut_tan.h"

ZDFLadder_t Moog_filter _CCM_;

void MoogLP_init(ZDFLadder_t * filter){
    filter->cutoff = 500.0f;
    filter->s1 = 0.0f;
    filter->s2 = 0.0f;
    filter->s3 = 0.0f;
    filter->s4 = 0.0f;
    filter->k = 3.5f;
}




float MoogLP_compute(ZDFLadder_t * filter, float input)
{
	float f, one_over_f, g, G, y1, y2, y3, y4, feedback;
    f = interp_lin_lut(LUT_TAN_SIZE * (PI * filter->cutoff * TS), LUT_TAN_SIZE, tan_lut);
    // f = tanf(PI * filter->cutoff * TS);
    //f = tan_lut[LUT_TAN_SIZE*(uint16_t)filter->cutoff];

	g = f / (1+f);
	G = g*g*g*g;
	// at output of 4th LP
	y4 = ((1/(1+f))*(filter->s4 + g*filter->s3 + g*g*filter->s2 + g*g*g*filter->s1) + G*input) / (1 + filter->k*G);
	// at other outputs
	feedback = (SoftClip(input) - SoftClip(filter->k*y4)); // good enough
	y1 = g*feedback + (filter->s1)/(1 + f);
	y2 = g*y1 + filter->s2/(1+f);
	y3 = g*y2 + filter->s3/(1+f);
	//update states s = y + v = y + f*(x-y) (=2v + s = 2*f(x-y) + s)
	filter->s1 = y1 + f*(feedback - y1);
	filter->s2 = y2 + f*(y1 - y2);
	filter->s3 = y3 + f*(y2 - y3);
	filter->s4 = y4 + f*(y3 - y4);
	return(y4);
}