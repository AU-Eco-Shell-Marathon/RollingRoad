/*******************************************************************************
* File Navn: functions.h
*
* Version: 3.0
*
* Description:
*  Indeholder funkitonsdefinitioner
*******************************************************************************/
#include <project.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define fs  20000				                       //sample frequency
#define N   100                                 	   //samples

//definitions to the log
#define update_time 0.2                                //the time between data is saved in the log
#define store_time 60                                  //the time data is stored in the log in seconds
#define log_len 300                                    //amount of spaces in the log 

//definitions to calculations
#define V_gain 10.4372//10.549                         //has to be multiplied with the sampled voltage
#define V_add 0.00038188//0.0013
#define I_gain 22.3165//22.555                         //has to be multiplied with the sampled current
#define I_add 2.65294//2.6301                          //has to be added to both the SAMPLED and LOAD current
#define I_load_gain 22.472                             //has to be multiplied with the load current
#define I_load_add 2.624
#define T_gain 1.9376                                  //has to be multiplied with the sampled torque
#define T_add 2.6097                                   //has to be added to the sampled torque
#define I_load_max 13.5                                //the maximum load current in the load circuit


static float32 set_torque;                             //torque setpoint set in the user interface

//definitions to UART messages
enum
{
    //actual data on PSoC
    get_speed              = 'E',  //OK
    get_torque             = 'F',  //OK
    get_pm                 = 'G',  //OK
    get_pe                 = 'H',  //OK
    get_efficiency         = 'I',  //OK
    get_power_usage        = 'R',  //OK
    get_set_torque         = 'J',  //OK
    get_mode               = 'Q',  //OK 
    
    //constant torque
    torque_plus_one        = 'L',  //OK
    torque_minus_one       = 'N',  //OK
    torque_minus_point_one = 'M',  //OK
    torque_plus_point_one  = 'K',  //OK
    torque_test_start      = 'O',  //OK
    torque_test_stop       = 'P',  //OK
    get_last_data          = 'D',  //OK

    //transmit functions used for the user interface
    new_line	    	   =  10,
	enter       		   =  13,
    
    //used to end messages sent to the user interface
    end				= '#',
};



void init(void);
float32 Efficiency(float32 Pout, float32 Pin);
float32 Mean(const float32 *x, const int16 len);
float32 Pe(float32 V, float32 I);
float32 Pm(float32 torque, float32 omega);
float32 Power_usage(float32 power, float32 time);
void Regulator(float32 Curr_torque);
void Send_log(const float32 *x_, const int16 log_lenght_, const int16 log_exceed_);