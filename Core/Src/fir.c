#include "../Inc/fir.h"

uint16_t avg_fliter16_t(uint16_t *buf ,int length){
    int32_t total=0;
    for (int i = 0; i < length; i++)
    {
        total=total + *(buf+i);
    }
    total=total/length;

    return (uint16_t)total;
}
