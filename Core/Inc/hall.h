#ifndef __HALL_H__
#define __HALL_H__

#include "main.h"

typedef struct
{
    uint8_t state;
    float angle;
    float speed;
    float speed_filter;
    float angle_inc;
    float speed_ref;
    uint32_t ccr;
} Hall_Typedef;

void Hall_Init(void);
void SysTick_ISR(void);
void Hall_SpeedFilter(void);
void BubbleSort(float* speed, uint8_t length);

extern Hall_Typedef Hall;
#endif
