

#ifndef _FLANGER_H
#define _FLANGER_H


#include <stdint.h>
#include "CONSTS.h"
#include "helper_functions.h"


#define FLANGER_BUFF_SIZE           1000  
#define INIT_FL_DEL                 220  
#define INIT_FL_FEEDB               0.9f
#define INIT_FLANGER_WET		   	0.5f

void flanger_init(void);

float flanger_compute(float input_sample);

#endif // !_FLANGER_H

