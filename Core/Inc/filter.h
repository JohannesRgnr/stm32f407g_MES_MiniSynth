#ifndef _FILTER_H
#define _FILTER_H


enum filter_type
{
	FILTER_LP=1,
	FILTER_HP,
	FILTER_BP,
	FILTER_NOTCH,
	FILTER_PEAK
};



typedef struct
{
	uint8_t 	type; // filter type : LP, HP, BP...
	float 		f;	/**< cutoff */
	float 		g;  /**< embedded integrator gain (Fig 3.11), wc == wa*/
	float 		q;	/**< q value calculated from setReso()*/
	float 		s1;
	float 		s2;
	float 		drive;
} filter;


#endif // !_FILTER_H
