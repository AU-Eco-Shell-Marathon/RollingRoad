/*******************************************************************************
* File Navn: main.c
*
* Version: 1.0
*
*
* Bskrivelse:
* Koden er skrevet i anledning af EPBAC7 efterår 2015.  
* Projektet er udviklet til PSoC 5LP.
*
*  Parametre brugt:
*   Opløsning:       12 bit
*   Clock Frekvens:  1.6 MHz
*   Sample Mode:     Hardware Trigger
*   Clock Source:    Internal
*   Input Range:     Vssa to Vdda (Single Ended)
*   Reference:       Internal Vref
*   Kanaler:         4
*******************************************************************************/
#include "functions.h"

float32	V[N];					            // array to the sampled values for the voltage 
float32 I[N];					            // array to the sampled values for the current
float32 I_load[N];                          // array to the sampled values for the load current
float32 Torque[N];                          // array to the sampled values for the torque


float32 V_mean;					            // the average value of the voltage
float32 V_corr;                             // the corrected voltage
float32 V_actual;                           // the actual value of the voltage

float32 I_mean;					            // the average value of the voltage representaion of the current
float32 I_corr;                             // the corrected current
float32 I_actual;                           // the actual value of the current

float32 I_load_mean;                        // the average value of the voltage representaion of the load current
float32 I_load_corr;                        // the corrected load current
float32 I_load_actual;                      // actual load current

float32 Torque_mean;                        // the average value of the torque
float32 Torque_corr;                        // the corrected torque
float32 Torque_actual;                      // actual torque

float32 Torque_mean_log[log_len];           // log for the torque (Nm)
float32 speed_log[log_len];                 // log for the speed (rpm)
float32 Pm_mean_log[log_len];               // log for the mechanical power (W)
float32 Pe_mean_log[log_len];               // log for the electric power input (W)
float32 efficiency_log[log_len];            // log for the efficiency (%)
float32 power_usage_log[log_len];           // log for the power usage (Wh)
       
volatile uint8 compare_occured=0;           // variable to count amounts of pulses in the counter
float32 shaft_speed;

int16 log_lenght=0;                         // this variable keeps track of which place in the log that are to be used next time
int16 log_exceed=0;                         // this variable is triggered when the log has been exceeded
static int16 mode=torque_test_stop;         // variable that states whether the test is ongoing or not

CY_ISR(isr_1) // sampling and processering is done here
{
	static long    a=0;
	    
	if(a<N)
	{
		V[a]      = AD_konverter_CountsTo_Volts(AD_konverter_GetResult16(0)); // the input voltage is sampled
		I[a]      = AD_konverter_CountsTo_Volts(AD_konverter_GetResult16(1)); // the input current is sampled
        Torque[a] = AD_konverter_CountsTo_Volts(AD_konverter_GetResult16(2)); // the input torque is sampled
        I_load[a] = AD_konverter_CountsTo_Volts(AD_konverter_GetResult16(3)); // the input load current is sampled
        
		a++;
	}
	else if(a==N)
	{	
		isr_1_Stop();
        
        V_mean        = Mean(V,N);				          // average value of the voltage is being calculated
        
        V_actual      = (V_mean-V_add)*V_gain;
        
        
        I_mean        = Mean(I,N);				           // the average value of the voltage representaion of the current
        
        I_actual      = (I_mean-I_add)*I_gain;
       
        
        I_load_mean   = Mean(I_load,N);                     // the average value of the voltage representaion of the load current
       
        I_load_actual = (I_load_mean-I_load_add)*I_load_gain;
        
        if(I_load_actual>I_load_max){                       // safety in case of overcurrent in the load circuit
                AD_konverter_Stop();
	            Sample_Clock_Stop();
                Counter_Clock_Stop();
                Counter_Speed_Stop();
                Speed_Clock_Stop();
                PWM_Speed_Stop();
                Speed_Clock_Stop();
                PWM_Regulering_Stop();
                PWM_Clock_Stop();
                Regulation_Clock_Stop();
	            Log_Clock_Stop();
            mode=torque_test_stop;    
        }
        
        Torque_mean = Mean(Torque,N);                       // average value of the torque is being calculated      
        
        if(Torque_mean>=T_add)
        {
            Torque_actual=(Torque_mean-T_add)*T_gain;      // for positive torque
        }
        else if(Torque_mean<T_add)
        {
            Torque_actual=(-T_gain*Torque_mean)+(2*T_add); // for "negative" torque
        }       
        
        a++;
		isr_1_StartEx(isr_1);
	}
	else
	{
		float32 delay=0.5;		// a delay before the sampling starts again. This makes some of the calculations more precise
		if(a<(N+(delay*fs)))
			a++;
		else
			a=0;
    }					
}

CY_ISR(isr_2) // speed pulse count and shaft speed calculation
{
    PWM_Speed_ReadStatusRegister(); // call the ReadStatusRegister to clear the Interrupt Status bit
    uint32 result = Counter_Speed_ReadCapture();
    shaft_speed=(2*M_PI*result*10)/360;   
}


CY_ISR(isr_3) // log variables are initialized here with the corresponding log values
{
    if(log_lenght==log_len)
    {
        log_exceed=1;
        log_lenght=0;
        
    }
      Torque_mean_log[log_lenght]=Torque_actual; // torque (Nm)
      speed_log[log_lenght]=shaft_speed; // speed (rad/sek)
      Pm_mean_log[log_lenght]=Pm(Torque_actual,shaft_speed); // mechanical power (W)
      Pe_mean_log[log_lenght]=Pe(V_actual,I_actual); // electric added power (W)
      efficiency_log[log_lenght]=Efficiency(Pm(Torque_actual,shaft_speed),Pe(V_actual,I_actual)); // efficiency (n)
      power_usage_log[log_lenght]=Power_usage(Pe(V_actual,I_actual),update_time); // electric power usage (Wh)
    
      log_lenght++;
}


CY_ISR(isr_4) // regulation is done here
{
   Regulator(Torque_actual);
}


CY_ISR(isr_5) // contains the communication protocols 
{
    char buf[100];
    
    switch(UART_ReadRxData())
	{   
		case get_torque:
        Send_log(Torque_mean_log, log_lenght, log_exceed);
            break;
		case get_speed:
        Send_log(speed_log, log_lenght, log_exceed);
            break; 
        case get_pm:
        Send_log(Pm_mean_log, log_lenght, log_exceed);    
            break;        
        case get_pe:   
        Send_log(Pe_mean_log, log_lenght, log_exceed);
            break;
        case get_efficiency:
        Send_log(efficiency_log, log_lenght, log_exceed);    
            break;
        case get_power_usage:
        Send_log(power_usage_log, log_lenght, log_exceed);
            break;
        case torque_plus_one:
			set_torque+=1.0;
            if(5<set_torque) set_torque = 5;
			break;
        case torque_minus_one:
			set_torque-=1.0;
            if(set_torque<0) set_torque = 0;
			break;
        case torque_plus_point_one:
			set_torque+=0.1;
            if(5<set_torque) set_torque = 5;
			break;
        case torque_minus_point_one:
			set_torque-=0.1;
            if(set_torque<0) set_torque = 0;
			break;
        case get_set_torque:
			sprintf(buf,"%.3f",set_torque);
			UART_PutString(buf);
            UART_PutChar(end);
			break;
        case torque_test_start:
	            AD_konverter_Start();
                Sample_Clock_Start();
                Counter_Clock_Start();
                Counter_Speed_Start();
                Speed_Clock_Start();
                PWM_Speed_Start();
                Speed_Clock_Start();
                PWM_Regulering_Start();
                PWM_Clock_Start();
                Regulation_Clock_Start();
	            Log_Clock_Start();
                mode=torque_test_start;            
            break;
        case torque_test_stop:
                AD_konverter_Stop();
	            Sample_Clock_Stop();
                Counter_Clock_Stop();
                Counter_Speed_Stop();
                Speed_Clock_Stop();
                PWM_Speed_Stop();
                Speed_Clock_Stop();
                PWM_Regulering_Stop();
                PWM_Clock_Stop();
                Regulation_Clock_Stop();
	            Log_Clock_Stop();
                mode=torque_test_stop;

            break;
        case get_mode:
                sprintf(buf,"%s%c",mode==torque_test_stop?"Stoppet":"Startet",end);
			    UART_PutString(buf);
            break;
        case get_last_data:
            //sprintf(buf,"V = %.3f og I = %.3f%c%c",V_actual,I_actual,enter,new_line);
			//UART_PutString(buf);			
            sprintf(buf,"Moment = %.3f Newton-meter%c%c",Torque_actual,enter,new_line);
			UART_PutString(buf);
			sprintf(buf,"Hastighed = %.3f rad per sekund%c%c",shaft_speed,enter,new_line);
			UART_PutString(buf);
			sprintf(buf,"Mekanisk effekt = %.3f Watt%c%c",Pm(Torque_actual,shaft_speed),enter,new_line);
			UART_PutString(buf);
			sprintf(buf,"Elektrisk effekt = %.3f Watt%c%c",Pe(V_actual,I_actual),enter,new_line);
			UART_PutString(buf);            
			sprintf(buf,"Virkningsgrad = %.3f %c%c",Efficiency(Pm(Torque_actual,shaft_speed),Pe(V_actual,I_actual)),enter,new_line);
			UART_PutString(buf);
			sprintf(buf,"Elektrisk energiforbrug = %.4f Watt-timer%c%c",Power_usage(Pe(V_actual,I_actual),update_time),enter,new_line);
			UART_PutString(buf);
			UART_PutChar(end);
			break;      
        default:
			break;            
    }   
}

int main()
{
	init();
	// Initializing the ISR
	CyGlobalIntEnable;
    isr_1_StartEx(isr_1); 
	isr_2_StartEx(isr_2);
	isr_3_StartEx(isr_3);
	isr_4_StartEx(isr_4);
    isr_5_StartEx(isr_5);	
    
    
    
    for(;;);
}