/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "PID.h"
#include <project.h>
#define kb 1


static struct PIDparameter parameter_;

//float PIDval = 0;
float dt = dt_def;
float iState = 0;
float anti_windup_back_calc = 0;
float err = 0;
float pre_err = 0;

struct PIDparameter *getPID_ptr()
{
    return &parameter_;
}

void PID_init()
{
    parameter_.Kp = Kp_def;
    parameter_.Ki = Ki_def;
    parameter_.Kd = Kd_def;
    parameter_.MAX = MAX_def;
    parameter_.MIN = MIN_def;
    parameter_.iMAX = iMAX_def;
    parameter_.iMIN = iMIN_def;
    
    PWM_1_Start();
    PWM_1_WriteCompare((unsigned char)parameter_.MIN);
    
}

float PID_debug[3];

float *PID_tick(float sensor, float input)
{
    float PIDval = 0;
    
    err = (input - sensor);
	
    PID_debug[1] = err;
    
	PIDval += parameter_.Kp*err; //Proportional calc.
	
    
    iState += parameter_.Ki*err*dt + anti_windup_back_calc;
    
    
	PIDval += iState; //intergral calc
	
	PIDval += parameter_.Kd*((err-pre_err)/dt); //differentiel calc
	
	pre_err = err;
	
    anti_windup_back_calc = PIDval;
    
	if(PIDval > parameter_.MAX)
		PIDval = parameter_.MAX;
    else if(PIDval < parameter_.MIN)
        PIDval = parameter_.MIN;
	
    anti_windup_back_calc = PIDval - anti_windup_back_calc;
    
    PID_debug[2] = anti_windup_back_calc;
    
    PID_debug[0] = PIDval;
    
	PWM_1_WriteCompare((uint8)PIDval);
    return PID_debug;
}

void setPID(const struct PIDparameter * parameter)
{
    parameter_ = *parameter;
}

/* [] END OF FILE */
