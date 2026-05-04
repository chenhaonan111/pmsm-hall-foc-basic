#include "pid.h"

Current_PID_DEF Current_D_PID;
Current_PID_DEF Current_Q_PID;
Speed_PID_DEF Speed_PID;

void PID_Init(void)
{
    rtX.Id_ref = 0.0f;
    rtX.Iq_ref = 1.5f;
    
    Current_D_PID.Kd = 0.0f;
    Current_D_PID.Ki = 0.005f;
    Current_D_PID.Kp = 1.0f;
    Current_D_PID.Kb = 0.005f;
    Current_D_PID.err = 0.0f;
    Current_D_PID.Dowm_Limit = -5;
    Current_D_PID.Up_Limit = 5;
    
    Current_Q_PID.Kd = 0.0f;
    Current_Q_PID.Ki = 0.005f;
    Current_Q_PID.Kp = 1.0f;
    Current_Q_PID.Kb = 0.005f;
    Current_Q_PID.err = 0.0f;
    Current_Q_PID.Dowm_Limit = -5;
    Current_Q_PID.Up_Limit = 5;
    
    Speed_PID.Kd = 0.0f;
    Speed_PID.Ki = 0.0001f;
    Speed_PID.Kp = 0.19f;
    Speed_PID.Kb = 1.0f;
    Speed_PID.err = 0.0f;
    Speed_PID.Dowm_Limit = -5;
    Speed_PID.Up_Limit = 5;
}

void Current_PID_Calc(float ref, float input, float* output, Current_PID_DEF* current_pid)
{
    float temp;
    
    current_pid->err = ref - input;
    
    current_pid->err_Kp = current_pid->Kp * current_pid->err;
    
    current_pid->err_Ki = current_pid->Ki * current_pid->err_Sum;
    
    temp = current_pid->err_Kp + current_pid->err_Ki;
    
    if(temp > current_pid->Up_Limit)
        *output = current_pid->Up_Limit;
    else if(temp < current_pid->Dowm_Limit)
        *output = current_pid->Dowm_Limit;
    else
        *output = temp;
        
    current_pid->err_Sum += current_pid->err + current_pid->Kb * (*output - temp);
}

void Speed_PID_Calc(float ref, float input, float* output, Speed_PID_DEF* speed_pid)
{
    float temp;
    
    speed_pid->err = ref - input;
    
    speed_pid->err_Kp = speed_pid->Kp * speed_pid->err;
    
    speed_pid->err_Ki = speed_pid->Ki * speed_pid->err_Sum;
    
    temp = speed_pid->err_Kp + speed_pid->err_Ki;
    
    if(temp > speed_pid->Up_Limit)
        *output = speed_pid->Up_Limit;
    else if(temp < speed_pid->Dowm_Limit)
        *output = speed_pid->Dowm_Limit;
    else
        *output = temp;
        
    speed_pid->err_Sum += speed_pid->err + speed_pid->Kb * (*output - temp);
}


