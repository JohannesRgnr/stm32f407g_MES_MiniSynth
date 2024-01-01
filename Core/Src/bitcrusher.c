

#include "bitcrusher.h"



float cnt = 0;


float decimator(float sample, float rate, int bit){
    float sampRate = rate * 0.995f + 0.005f; 
    uint32_t m = 1 << (bit - 1);
    cnt += sampRate;
    if (cnt >= 1)
    {
        cnt -= 1;
        return (int32_t)(sample * m) / (float)m;
        } 
}