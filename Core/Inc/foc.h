#ifndef __FOC_H__
#define __FOC_H__

#include "main.h"
#include "math.h"

#define PI          3.14159265358979f

typedef struct
{
    float theta;        /*转子角度*/
    float Udc;          /*电机控电电压*/
    float Tpwm;         /*电机PWM周期*/
    float Id_ref;
    float Iq_ref;
} ExtX;

typedef struct
{
    float Vd;
    float Vq;
} Voltage_DQ_DEF;

typedef struct
{
    float Cos;
    float Sin;
} Transf_Cos_Sin_DEF;

typedef struct
{
    float Valpha;
    float Vbeta;
} Voltage_Alpha_Beta_DEF;

typedef struct
{
    float Ia;           /* 电机U项电流 */
    float Ib;           /* 电机V项电流 */
    float Ic;           /* 电机W项电流 */
} Current_ABC_DEF;

typedef struct
{
    float Ialpha;           /* Clarke变换输出Alpha项电流 */
    float Ibeta;            /* Clarke变换输出Beta项电流 */
} Current_Alpha_Beta_DEF;

typedef struct
{
    float Id;           /* Park变换输出d轴电流 */
    float Iq;           /* Park变换输出q轴电流 */
} Current_DQ_DEF;



void FOC_Init(void);
void FOC_Run(void);
void Angle_To_Cos_Sin(float angle, Transf_Cos_Sin_DEF* cos_sin);
void Rev_Park_Transf(Voltage_DQ_DEF v_dq, Transf_Cos_Sin_DEF cos_sin, Voltage_Alpha_Beta_DEF* v_alpha_beta);
void SVPWM_Calc(Voltage_Alpha_Beta_DEF v_alpha_beta, float Udc, float Tpwm);
void Clarke_Transf(Current_ABC_DEF Current_abc, Current_Alpha_Beta_DEF* Current_alpha_beta);
void Park_Transf(Current_Alpha_Beta_DEF Current_alpha_beta, Transf_Cos_Sin_DEF cos_sin, Current_DQ_DEF* current_dq);

extern ExtX rtX;                                       /* 输入结构体变量 */
extern Transf_Cos_Sin_DEF Transf_Cos_Sin;              /* 当前角度计算三角函数 */
extern Voltage_DQ_DEF Voltage_DQ;                      /* DQ电压输出 */
extern Voltage_Alpha_Beta_DEF Voltage_Alpha_Beta;      /* RevPark变换输出 */
extern Current_ABC_DEF Current_Iabc;                   /* 当前UVW三项电流 */
extern Current_Alpha_Beta_DEF Current_Ialpha_beta;     /* Clarke变换输出 */
extern Current_DQ_DEF Current_Idq;                     /* Park变换输出 */



#endif

