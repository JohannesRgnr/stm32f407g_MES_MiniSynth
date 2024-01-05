/**
 * @file filters.h
 * @author johannes regnier
 * @brief various filters
 * @version 0.1
 * @date 2024-01-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */



#ifndef _FILTERS_H_
#define _FILTERS_H_


typedef struct
{
	float s1;
	float s2;
	float r;
	float cutoff;
	float g;
} ZDFLP_t;

void  SVF_LP_init(ZDFLP_t *filter);
float freq_to_g(float freq); // calculate g coefficient for SVF
float SVF_LP_compute(ZDFLP_t *f, float sample);
float smoothing_LP(float input, float alpha);

#endif // !_FILTERS_H_
