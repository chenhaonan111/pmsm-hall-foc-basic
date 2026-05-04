#ifndef __PMSM_H__
#define __PMSM_H__

#include "main.h"
#include "math.h"
#include "foc.h"

#define CW              1
#define CCW             2

#define STATE_RUN           1
#define STATE_STOP          0
#define STATE_PRE_ALIGN     1
#define STATE_OPEN_LOOP     2
#define STATE_CLOSED_LOOP   3

#define ALIGN_CURRENT       1.0f
#define ALIGN_CYCLES        4000
#define OPENLOOP_CYCLES     4000

typedef struct
{
    uint8_t dir;
    uint8_t State;
    uint32_t cycle_count;
} Pmsm_Motor;

void Motor_Run(void);
void Motor_Init(void);
void Get_Temper(void);
void pmsm_state_init(void);


extern Pmsm_Motor pmsm;

#endif
