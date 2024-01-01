/**
 * @file ladder_filter.c
 * @author johannes regnier
 * @brief Zero delay feedback 4-pole ladder filter
 * @note Simple implementation following Vadim Zavalishin (2008) - Preserving the LTI system topology in s- to z-plane transforms
 * @version 0.1
 * @date 2023-12-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ladder_filter.h"
#include "helper_functions.h"
#include "CONSTS.h"
#include "lut_tan.h"

ZDFLadder_t Moog_filter _CCM_;


void MoogLP_init(ZDFLadder_t * filter){
    filter->cutoff = 100.0f;
    filter->s1 = 0.0f;
    filter->s2 = 0.0f;
    filter->s3 = 0.0f;
    filter->s4 = 0.0f;
    filter->k = 0.0f;
}




float MoogLP_compute(ZDFLadder_t * filter, float input)
{
	float f, one_over_oneplusf, g, G, y1, y2, y3, y4, feedback;
    f = interp_lin_lut(LUT_TAN_SIZE * (PI * filter->cutoff * TS), LUT_TAN_SIZE, tan_lut);
    // f = tanf(PI * filter->cutoff * TS);
	one_over_oneplusf = 1 / (1 + f);
	g = f * one_over_oneplusf;
	G = g*g*g*g;
	// at output of 4th stage
	y4 = ((one_over_oneplusf)*(filter->s4 + g*filter->s3 + g*g*filter->s2 + g*g*g*filter->s1) + G*input) / (1 + filter->k*G);
	// at other outputs
	feedback = (SoftClip(input) - SoftClip(filter->k*y4)); // naive but good enough
	y1 = g*feedback + (filter->s1) * one_over_oneplusf;
	y2 = g*y1 + filter->s2 * one_over_oneplusf;
	y3 = g*y2 + filter->s3 * one_over_oneplusf;
	//update states s = y + v = y + f*(x-y) (=2v + s = 2*f(x-y) + s)
	filter->s1 = y1 + f*(feedback - y1);
	filter->s2 = y2 + f*(y1 - y2);
	filter->s3 = y3 + f*(y2 - y3);
	filter->s4 = y4 + f*(y3 - y4);
	return(y4);
}