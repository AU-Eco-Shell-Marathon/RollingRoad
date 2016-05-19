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
#include <Math.h>
#include "Constants.h"
#define kb 1


static struct PIDparameter parameter_;

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
    
    PWM_1_Start();
    PWM_1_WriteCompare((unsigned char)parameter_.MIN);
    
}

float PID_debug[3];

float *PID_tick(float sensor, float input, float RPM)
{
    err = (input - sensor);
    
    float P_RPM = (P_RPM_reciprocal / RPM == infinityf() ? P_RPM_reciprocal : P_RPM_reciprocal / RPM) ;

    float PIDval = 0;
    
	//Proportional part
	PIDval += P_RPM*parameter_.Kp*err;
    
    //intergral part
    iState += P_RPM*parameter_.Kp*parameter_.Ki*err*dt + anti_windup_back_calc;
	PIDval += iState; 
	
    //differentiel
	PIDval += P_RPM*parameter_.Kp*parameter_.Kd*((err-pre_err)/dt);
	pre_err = err;
	
    //anti windup back calculation
    anti_windup_back_calc = PIDval;
    
	if(PIDval > parameter_.MAX)
		PIDval = parameter_.MAX;
    else if(PIDval < parameter_.MIN)
        PIDval = parameter_.MIN;
	
    anti_windup_back_calc = PIDval - anti_windup_back_calc;
    // --- //
    
    //use PIDval
	PWM_1_WriteCompare((uint8)PIDval);
    
    //Debug
    PID_debug[0] = PIDval;
    PID_debug[1] = err;
    PID_debug[2] = anti_windup_back_calc;
    return PID_debug;
}

void setPID(const struct PIDparameter * parameter)
{
    parameter_ = *parameter;
}

/* [] END OF FILE */
