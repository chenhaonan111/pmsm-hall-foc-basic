#include "hall.h"
#include "tim.h"

Hall_Typedef Hall;

void Hall_Init(void)
{
    __HAL_TIM_ENABLE_IT(&htim5, TIM_IT_TRIGGER);
    HAL_TIMEx_HallSensor_Start_IT(&htim5);
}

void HAL_TIM_TriggerCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM5)
    {
        static float HALL_CW_Arr[6] = {3.509F, 5.467F, 4.355F, 0.929F, 2.189F, 0.089F};
        static float HALL_CCW_Arr[6] = {4.021F, 5.925F, 4.861F, 2.011F, 3.181F, 0.801F};
        Hall.ccr += TIM5->CCR1;
        __HAL_TIM_SET_COUNTER(&htim5, 0);
        
        Hall.state = HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_10);
        Hall.state |= (HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_11) << 1);
        Hall.state |= (HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_12) << 2);

        if(pmsm.dir == CW)
        {
            Hall.angle = HALL_CW_Arr[Hall.state - 1];
            if(Hall.state == 2)
            {
                Hall.speed = 2*PI/Hall.ccr*1000000;
                Hall.ccr = 0;
            }
        }
        if(pmsm.dir == CCW)
        {
            Hall.angle = HALL_CCW_Arr[Hall.state - 1];
            if(Hall.state == 6)
            {
                Hall.speed = 2 * PI / Hall.ccr * 1000000;
                Hall.ccr = 0;
            }
        }
    }
}

void SysTick_ISR(void)
{
    Hall_SpeedFilter();
}

static void Hall_SpeedFilter(void)
{
    static uint8_t i;
    static float speed_mean,speed[10];

    speed[i++] = Hall.speed;		// 0 1 2 3 4 5 6 7 8 9
    if(i == 10)
    {
        i = 0;
        speed_mean = 0;
        BubbleSort(&speed[0],10);
        for(uint8_t j=2; j<8; j++)
            speed_mean += speed[j];
        speed_mean /= 6.0f;
    }
    Hall.speed_filter = 0.8F*speed_mean+0.2F*Hall.speed;
    Hall.angle_inc = Hall.speed_filter*0.0001f;
    
    if(Speed_PID.PID_FLAG == 1)
    {
        Speed_PID_Calc(Hall.speed_ref, Hall.speed_filter, &rtX.Iq_ref, &Speed_PID);
    }
}

void BubbleSort(float* speed, uint8_t length) 
{
    for (int i = 0; i < length - 1; i++) 
        for (int j = 0; j < length - 1 - i; j++) 
        {
            if (speed[j] > speed[j + 1])
            {
                float temp = speed[j];
                speed[j] = speed[j + 1];
                speed[j + 1] = temp;
            }
        }
}
