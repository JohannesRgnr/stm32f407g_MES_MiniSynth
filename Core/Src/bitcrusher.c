/**
 * @file bitcrusher.c
 * @author johannes regnier
 * @brief a classic bitcrusher + sample rate decimator
 * @version 0.1
 * @date 2024-01-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "bitcrusher.h"



float cnt = 0;


float decimator(float sample, float rate, int bit){
    float sampRate = rate * 0.995f + 0.005f;
    static float value;
    uint32_t m = 1 << (bit - 1); // bit - 1 --> signal goes max from -32767 to 32767
    cnt += sampRate;
    if (cnt >= 1) // sample and hold
    {
        cnt -= 1;
        value = (int32_t)(sample * m) / (float)m;
    }
    return value;
}