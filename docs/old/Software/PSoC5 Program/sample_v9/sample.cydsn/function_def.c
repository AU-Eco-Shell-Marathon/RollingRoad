#include "functions.h"

static float32 power_usage;


void init(void)
{
//    AD_konverter_Start();
//	Sample_Clock_Start();
//    Speed_Clock_Start();
//    pwm_Clock_Start();
//    PWM_Start();
//    Regulation_Clock_Start();
//	Log_Clock_Start();
    UART_Start();
    
}


float32 Efficiency(float32 Pout, float32 Pin) //calculates the efficiency by dividing the output and input power
{
    return Pout/Pin;
}


float32 Mean(const float32 *x, const int16 size) // returns the mean value of "sum", which has the lenght "size"
{
	float32 sum=0;
	int16 i=0;
	
		for (i=0;i<size;i++)
			sum+=x[i];
	return sum/((float32)size); 
}


float32 Pe(float32 V, float32 I) //calculates the electrical power added to the system
{
    return V*I;
}


float32 Pm(float32 torque, float32 omega) //calculates the mechanical power added to the system
{
    return torque*omega;
}

float32 Power_usage(float32 power, float32 time) //calculates the power usage per hour (Wh)
{
    return power_usage += ((power*time)/3600);
}

void Send_log(const float32 *x_, const int16 log_lenght_, const int16 log_exceed_)
{
	char buf[100];
	int16 i,j=0;
	
	if(log_exceed_)
	{
		j=1-log_len;
		for(i=log_lenght_;i<log_len;i++)
		{
			sprintf(buf,"%.1f %.2f%c%c",((float32)update_time)*j,x_[i],enter,new_line);
			UART_PutString(buf); //Places data from a string into the memory buffer for transmitting	
			j++;
		}
		for(i=0;i<log_lenght_;i++)
		{	
			sprintf(buf,"%.1f %.2f%c%c",((float32)update_time)*j,x_[i],enter,new_line);
			UART_PutString(buf); //Places data from a string into the memory buffer for transmitting	
			j++;
		}
	}
	else
	{
		for(i=0;i<log_lenght_;i++)
		{
			sprintf(buf,"%.1f %.2f%c%c",(((float32)update_time)*(j-log_lenght_+1)),x_[i],enter,new_line);
			UART_PutString(buf); //Places data from a string into the memory buffer for transmitting	
			j++;
		}
	}
	UART_PutChar(end);
}

const int Kp            = 5;        //proportional gain parameter
const int Ki            = 63;       //integral gain parameter 62.89=63
const int PROP_REG_LIM  = 25;       //working range of proportional element
const int INTL_REG_LIM  = 65457;    //working range of integral element

float32 Tref = 2.5;
float32 CurrError;                  //current error
float32 AccuError;                  //accumulated error
float32 PropTerm;                   //proportional term
float32 IntlTerm;                   //integral term

int PWM_output;

void Regulator(float32 Curr_torque)
{

   //CurrError = set_torque - Curr_torque;  // Current error
    CurrError = Tref - Curr_torque;  // Current error

   /* Calculate Proportional Term */
   PropTerm = CurrError * Kp;
   if(PropTerm > PROP_REG_LIM)  //whether the proportional term is below/over the proportional regulation limit
      PropTerm = PROP_REG_LIM;
      else{
         if(PropTerm < -PROP_REG_LIM)
            PropTerm = -PROP_REG_LIM;
      }
    
    /* Calculate Integral Term */
   AccuError += CurrError;
   if(AccuError > INTL_REG_LIM) AccuError = INTL_REG_LIM;
   if(AccuError < -INTL_REG_LIM) AccuError = -INTL_REG_LIM;

   IntlTerm = AccuError * Ki;
   if(IntlTerm > INTL_REG_LIM)  //whether the integral term is below/over the integral regulation limit
      IntlTerm = INTL_REG_LIM;
      else{
         if(IntlTerm < -INTL_REG_LIM)
            IntlTerm = -INTL_REG_LIM;
      }   
    PWM_output = (int)(PropTerm+IntlTerm)>>8;
    PWM_Regulering_WriteCompare(PWM_output);
}