#ifndef __PID_H__
#define __PID_H__

#include "main.h"

typedef struct
{
    float Kp;
    float Ki;
    float Kd;
    float Kb;
    float Up_Limit;
    float Dowm_Limit;
    float err;
    float err_Sum;
    float err_Kp;
    float err_Ki;
} Current_PID_DEF;

typedef struct
{
    uint8_t PID_FLAG;
    float Kp;
    float Ki;
    float Kd;
    float Kb;
    float Up_Limit;
    float Dowm_Limit;
    float err;
    float err_Sum;
    float err_Kp;
    float err_Ki;
} Speed_PID_DEF;

void PID_Init(void);
void Current_PID_Calc(float ref, float input, float* output, Current_PID_DEF* current_pid);
void Speed_PID_Calc(float ref, float input, float* output, Speed_PID_DEF* speed_pid);


extern Current_PID_DEF Current_D_PID;
extern Current_PID_DEF Current_Q_PID;
extern Speed_PID_DEF Speed_PID;

#endif

