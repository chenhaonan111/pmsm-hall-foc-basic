#include "pmsm.h"

float power = 0;
float temper = 0;

const float Rp = 10000.0f;
const float T2 = (273.15f + 25.0f);
const float Bx = 3380.0f;
const float Ka = 273.15f;

Pmsm_Motor pmsm;

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3;

extern TIM_HandleTypeDef htim1;

void Motor_Init(void)
{
    pmsm_state_init();

    FOC_Init();
    PID_Init();
    Hall_Init();

    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);

    HAL_ADCEx_InjectedStart_IT(&hadc1);
    HAL_ADCEx_InjectedStart(&hadc2);
    HAL_ADCEx_InjectedStart(&hadc3);

    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)ADC_DMABuff, 2);
}


void Motor_Run(void)
{
    static float theta;

//    power = (ADC_DMABuff[1] * 3.3f / 4096.0F) * 25.0F;

    switch (pmsm.State)
    {
    case STATE_STOP:
        return;

    case STATE_PRE_ALIGN:
        rtX.Id_ref = ALIGN_CURRENT;
        rtX.Iq_ref = 0.0f;
        Speed_PID.PID_FLAG = 0;

        if (pmsm.dir == CW)
            theta += 0.001257f;
        else
            theta -= 0.001257f;

        if (theta > 2*PI)       theta -= 2*PI;
        if (theta < 0)          theta += 2*PI;
        rtX.theta = theta;

        pmsm.cycle_count++;
        if (pmsm.cycle_count >= ALIGN_CYCLES)
        {
            Current_D_PID.err_Sum = 0.0f;
            Current_Q_PID.err_Sum = 0.0f;
            theta = 0.0f;
            pmsm.cycle_count = 0;
            pmsm.State = STATE_OPEN_LOOP;
        }
        break;

    case STATE_OPEN_LOOP:
        rtX.Id_ref = 0.0f;
        rtX.Iq_ref = 1.5f;
        pmsm.cycle_count++;

        if (pmsm.dir == CW)
            theta += 0.01F;
        if (pmsm.dir == CCW)
            theta -= 0.01F;
        if (theta > 2*PI)
            theta -= 2*PI;
        if (theta < 0)
            theta += 2*PI;
        rtX.theta = theta;

        if (pmsm.cycle_count >= OPENLOOP_CYCLES)
        {
            Current_D_PID.err_Sum = 0.0f;
            Current_Q_PID.err_Sum = 0.0f;
            Speed_PID.err_Sum = 0.0f;
            Speed_PID.PID_FLAG = 1;
            pmsm.State = STATE_CLOSED_LOOP;
        }
        break;

    case STATE_CLOSED_LOOP:
        Hall.angle += (pmsm.dir == CW) ? Hall.angle_inc : -Hall.angle_inc;
        if (Hall.angle > 2*PI)
            Hall.angle -= 2*PI;
        else if (Hall.angle < 0)
            Hall.angle += 2*PI;

        rtX.Id_ref = 0.0f;
        rtX.theta = Hall.angle;

        if (rtX.Iq_ref < 0.8f)
            rtX.Iq_ref = 0.8f;
        else if (rtX.Iq_ref > 3.5f)
            rtX.Iq_ref = 3.5f;
        break;
    }

    FOC_Run();
}

void pmsm_state_init(void)
{
    pmsm.dir = CW;
    pmsm.State = STATE_STOP;
    pmsm.cycle_count = 0;
}

void Get_Temper(void)
{
    float Rt;
    float temp;
    Rt = 3.3f * 4700.0f / (ADC_DMABuff[0] * 3.3f / 4096.0f) - 4700.0f;

    temp = Rt / Rp;
    temp = log(temp);
    temp /= Bx;
    temp += (1.0f / T2);
    temp = 1.0f / (temp);
    temp -= Ka;
    
    temper = temp;    
}
