/**
 * @file helper_functions.c
 * @author johannes regnier
 * @brief Various math tools 
 * @version 0.1
 * @date 2023-12-30
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "helper_functions.h"



/**
 * @brief Linear interpolation within a given lookup table. Useful for wavetable oscillators.
 * 
 * @param index 
 * @param table_size 
 * @param table 
 * @return float 
 */
float interp_lin_lut(float index, uint16_t table_size, const float *table){
    float diff;
    uint32_t trunc = (uint32_t)index; // truncate the index but don't overwrite
    float frac = index - trunc; // get the fractional part

    while (trunc > table_size)
        trunc = trunc - table_size;

    if (trunc == table_size-1) 
        diff = table[0] - table[trunc]; // wrap
    else
        diff = table[trunc+1] - table[trunc]; // no need to wrap

    // get the interpolated output
    return table[trunc] + (diff * frac);
}

/**
 * @brief Constrain input value between lower and upper limits
 * 
 * @param input 
 * @param lower 
 * @param upper 
 * @return float 
 */
float clip(float input, float lower, float upper) {
        return max(lower, min(input, upper));
}




