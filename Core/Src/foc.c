#include "foc.h"

ExtX rtX;
Transf_Cos_Sin_DEF Transf_Cos_Sin;
Voltage_DQ_DEF Voltage_DQ;
Voltage_Alpha_Beta_DEF Voltage_Alpha_Beta;

Current_ABC_DEF Current_Iabc;                   /* 当前UVW三项电流 */
Current_Alpha_Beta_DEF Current_Ialpha_beta;     /* Clarke变换输出 */
Current_DQ_DEF Current_Idq;                     /* Park变换输出 */


void FOC_Init(void)
{
    rtX.Udc = 24;
    rtX.Tpwm = 8400*2;
    Voltage_DQ.Vd = rtX.Id_ref;
    Voltage_DQ.Vq = rtX.Iq_ref;
    Hall.speed_ref = 200;
    Speed_PID.PID_FLAG = 0;
}

void FOC_Run(void)
{
    Current_Iabc.Ia = (ADC2 -> JDR1 * 3.3f / 4096.0F - 1.25f) / 0.12f;
    Current_Iabc.Ib = (ADC1 -> JDR1 * 3.3f / 4096.0F - 1.25f) / 0.12f;
    Current_Iabc.Ic = (ADC3 -> JDR1 * 3.3f / 4096.0F - 1.25f) / 0.12f;
    
    Clarke_Transf(Current_Iabc, &Current_Ialpha_beta);
    Angle_To_Cos_Sin(rtX.theta, &Transf_Cos_Sin);
    Park_Transf(Current_Ialpha_beta, Transf_Cos_Sin, &Current_Idq);
    
    Current_PID_Calc(rtX.Id_ref, Current_Idq.Id, &Voltage_DQ.Vd, &Current_D_PID);
    Current_PID_Calc(rtX.Iq_ref, Current_Idq.Iq, &Voltage_DQ.Vq, &Current_Q_PID);
    
    Rev_Park_Transf(Voltage_DQ, Transf_Cos_Sin, &Voltage_Alpha_Beta);
    SVPWM_Calc(Voltage_Alpha_Beta, rtX.Udc, rtX.Tpwm);
}

void Angle_To_Cos_Sin(float angle, Transf_Cos_Sin_DEF* cos_sin)
{
    cos_sin->Cos = cosf(angle);
    cos_sin->Sin = sinf(angle);
}

void Rev_Park_Transf(Voltage_DQ_DEF v_dq, Transf_Cos_Sin_DEF cos_sin, Voltage_Alpha_Beta_DEF* v_alpha_beta)
{
    v_alpha_beta->Valpha = cos_sin.Cos * v_dq.Vd - cos_sin.Sin * v_dq.Vq;
    v_alpha_beta->Vbeta  = cos_sin.Sin * v_dq.Vd + cos_sin.Cos * v_dq.Vq;
}

void SVPWM_Calc(Voltage_Alpha_Beta_DEF v_alpha_beta, float Udc, float Tpwm)
{
    int sector = 0;
    float Tx, Ty, Ta, Tb, Tc;

    sector = 0;
    if(v_alpha_beta.Vbeta > 0.0F)
        sector = 1;
    if((1.73205078F * v_alpha_beta.Valpha - v_alpha_beta.Vbeta) / 2.0F > 0.0F)
        sector += 2;
    if((-1.73205078F * v_alpha_beta.Valpha - v_alpha_beta.Vbeta) / 2.0F > 0.0F)
        sector += 4;
    
    switch (sector) 
    {
        case 1:
            Tx = (-1.5F * v_alpha_beta.Valpha + 0.866025388F * v_alpha_beta.Vbeta) * (Tpwm / Udc);
            Ty = (1.5F * v_alpha_beta.Valpha + 0.866025388F * v_alpha_beta.Vbeta) * (Tpwm / Udc);
            break;

        case 2:
            Tx = (1.5F * v_alpha_beta.Valpha + 0.866025388F * v_alpha_beta.Vbeta) * (Tpwm / Udc);
            Ty = -(1.73205078F * v_alpha_beta.Vbeta * Tpwm / Udc);
            break;

        case 3:
            Tx = -((-1.5F * v_alpha_beta.Valpha + 0.866025388F * v_alpha_beta.Vbeta) * (Tpwm / Udc));
            Ty = 1.73205078F * v_alpha_beta.Vbeta * Tpwm / Udc;
            break;

        case 4:
            Tx = -(1.73205078F * v_alpha_beta.Vbeta * Tpwm / Udc);
            Ty = (-1.5F * v_alpha_beta.Valpha + 0.866025388F * v_alpha_beta.Vbeta) * (Tpwm/ Udc);
            break;

        case 5:
            Tx = 1.73205078F * v_alpha_beta.Vbeta * Tpwm / Udc;
            Ty = -((1.5F * v_alpha_beta.Valpha + 0.866025388F * v_alpha_beta.Vbeta) * (Tpwm/ Udc));
            break;

        default:
            Tx = -((1.5F * v_alpha_beta.Valpha + 0.866025388F * v_alpha_beta.Vbeta) * (Tpwm/ Udc));
            Ty = -((-1.5F * v_alpha_beta.Valpha + 0.866025388F * v_alpha_beta.Vbeta) * (Tpwm/ Udc));
            break;
    }
    
    float T = Tx + Ty;
    if (T > Tpwm)
    {
        float scale = Tpwm / T;
        Tx *= scale;
        Ty *= scale;
    }
    
    Ta = (Tpwm - (Tx + Ty)) / 4.0F;
    Tb = Tx / 2.0F + Ta;
    Tc = Ty / 2.0F + Tb;
    
    switch (sector)
    {
        case 1:
            TIM1->CCR1 = Tb;
            TIM1->CCR2 = Ta;
            TIM1->CCR3 = Tc;
            break;
            
        case 2:
            TIM1->CCR1 = Ta;
            TIM1->CCR2 = Tc;
            TIM1->CCR3 = Tb;
            break;
            
        case 3:
            TIM1->CCR1 = Ta;
            TIM1->CCR2 = Tb;
            TIM1->CCR3 = Tc;
            break;
            
        case 4:
            TIM1->CCR1 = Tc;
            TIM1->CCR2 = Tb;
            TIM1->CCR3 = Ta;
            break;
            
        case 5:
            TIM1->CCR1 = Tc;
            TIM1->CCR2 = Ta;
            TIM1->CCR3 = Tb;
            break;
            
        case 6:
            TIM1->CCR1 = Tb;
            TIM1->CCR2 = Tc;
            TIM1->CCR3 = Ta;
            break;
    }
}

/*Clarke变换*/
void Clarke_Transf(Current_ABC_DEF Current_abc, Current_Alpha_Beta_DEF* Current_alpha_beta)
{
    Current_alpha_beta->Ialpha = Current_abc.Ia;
    Current_alpha_beta->Ibeta = (Current_abc.Ia + Current_abc.Ib * 2.0F) * 0.577350269F;
}

/*Park变换*/
void Park_Transf(Current_Alpha_Beta_DEF Current_alpha_beta, Transf_Cos_Sin_DEF cos_sin, Current_DQ_DEF* current_dq)
{
    current_dq->Id = Current_alpha_beta.Ialpha * cos_sin.Cos + Current_alpha_beta.Ibeta * cos_sin.Sin;
    current_dq->Iq = -Current_alpha_beta.Ialpha * cos_sin.Sin + Current_alpha_beta.Ibeta * cos_sin.Cos;
}



