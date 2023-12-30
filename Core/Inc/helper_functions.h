/**
 * @file helper_functions.h
 * @author johannes regnier
 * @brief Various math tools 
 * @version 0.1
 * @date 2023-12-30
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _HELPER_FUNCTIONS_H
#define _HELPER_FUNCTIONS_H


#include <math.h>
#include <stdint.h>

#ifndef NOMINMAX

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#endif  /* NOMINMAX */

/**
 * @brief Keep value between 0 and max
 * 
 * @param value 
 * @param max 
 * @return float 
 */
inline float wrap(float value, float max)   
{
    if (value < 0)
        value += max;
    if (value >= max)
        value -= max;

    return value;
}

inline float Crossfade(float a, float b, float fade)
{
    return a + (b - a) * fade;
}


/**
 * @brief Pade-approximation of tanh for soft clipping
 * @ from https://www.musicdsp.org/en/latest/Other/238-rational-tanh-approximation.html 
 * @param x 
 * @return float 
 */
inline float SoftClip(float x)
{
    if (x < -3.0f)
    {
        return -1.0f;
    }
    else if (x > 3.0f)
    {
        return 1.0f;
    }
    else
    {
        return x * (27.0f + x * x) / (27.0f + 9.0f * x * x);
    }
}


float interp_lin_lut(float index, uint16_t table_size, const float *table); // linear interpolation within a given lookup table
float clip(float input, float lower, float upper); // constrain value between lower and upper values

#endif // !_HELPER_FUNCTIONS_H
