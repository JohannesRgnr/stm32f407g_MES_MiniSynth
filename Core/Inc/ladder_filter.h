#ifndef _LADDER_FILTER_H
#define _LADDER_FILTER_H



typedef struct
{
	float cutoff;	// cutoff frequency (Hz)
	float s1;
	float s2;
	float s3;
	float s4;
	float k;
}	ZDFLadder_t;

void MoogLP_init(ZDFLadder_t *filter);
float MoogLP_compute(ZDFLadder_t *filter, float input);

#endif // !_LADDER_FILTER_H
