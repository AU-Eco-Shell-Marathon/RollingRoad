#include <project.h>
#include "Uart.h"
#include "ControllerClass.h"
#include "PID.h"
#include "sensor.h"
#include "EEPROM.h"

uint8_t buf[250] = {0};
uint8 buf_n = 0;

char isReadyToSend = 0;//skal være 0 

char string[] = "dummy\n";
char handshake[] = "0 RollingRoad\n";
char torque[20];
char modtaget[64];

void InitUart(void)
{
    USBUART_1_Start(0, USBUART_1_5V_OPERATION);
    
    while(CheckConnection()==0u);
    //while(USBUART_1_DataIsReady() == 0);
    //while(USBUART_1_CDCIsReady() == 0u);

}

char CheckConnection(void)
{
    if(USBUART_1_IsConfigurationChanged() == 0)
        return 0;
    if(USBUART_1_GetConfiguration() == 0)
        return 0;
    USBUART_1_CDC_Init();  
    return 1;
}

void ReceiveUARTData(void)
{
    CheckConnection();
    
    if(USBUART_1_GetConfiguration() == 0)
        return;
    
    if( USBUART_1_DataIsReady() == 0)
        return;
    
    USBUART_1_GetAll(buf);
    if(buf_n==0 && ( buf[0]>='0' && buf[0] <='7'))
    {
        if(buf[0]=='0') // Handshake
        { 
            if(strncmp((char*)buf, handshake, sizeof(handshake))==0)
            {
                
                struct PIDparameter PIDval = *getPID_ptr();
                
	            SendUART(handshake);
                CyDelay(1);
                SendUART("1 0 Time ms\n");
                CyDelay(1);
                SendUART("1 1 Force N\n");
                CyDelay(1);
                SendUART("1 2 Voltage V\n");
                CyDelay(1);
                SendUART("1 3 Ampere A\n");
                CyDelay(1);
                SendUART("1 4 Effect(EL) W\n");
                CyDelay(1);
                SendUART("1 5 Distance m\n");
                CyDelay(1);
                SendUART("1 6 Speed m/s\n");
                CyDelay(1);
                SendUART("1 7 Effect(Mek) W\n");
                CyDelay(1);
                SendUART("1 8 efficiency procent\n");
                CyDelay(1);
                SendUART("1 9 Overcurrent\n");
                CyDelay(1);
                SendUART("1 10 Force_set (N)\n");
                CyDelay(1);
                SendUART("1 11 PIDval\n");
                CyDelay(1);
                SendUART("1 12 PIDerr\n");
                CyDelay(1);
                SendUART("1 13 PIDanti_windup\n");
                CyDelay(1);
                SendUART("1 14 PIDinput\n");
                CyDelay(1);
                SendUART("1 15 PIDsensor\n");
                CyDelay(1);
                SendUART("1 16 ForceRMS N\n");
                CyDelay(1);
                SendUART("1 17 VoltageRMS V\n");
                CyDelay(1);
                SendUART("1 3 AmpereRMS A\n");
                CyDelay(1);
                char buf[50];
                sprintf(buf, "5 %f %f %f\n",
                    PIDval.Kp,
                    PIDval.Ki,
                    PIDval.Kd
                );
                SendUART(buf);
                
                isReadyToSend = 1;
               // CyDelay(100);    HUSK AF OPDATER MED SENESTE PROTOKOL
               // SendData((uint8*)"1 2 Voltage Volt\n");
            }
        }
        
        else if(buf[0]=='2') // Stop
        {
            buf[buf_n+1]=0;
            if(strcmp((char*)buf, "2\n")==0)
            {
                isReadyToSend = 0;
                return;
            }
        }
        else if(buf[0]=='4') //modtag Force fra PC
        {
            
            float moment = 0;
            uint8 i;
            
            
            
            for(i = 0; i< 250; i++)
            {
                if(buf[i] == '\n' || buf[i] == '\r' || buf[i] == 0)
                {
                    buf[i] = 0;
                    break;
                }
            }
            
            //strtok((char*)buf, " ");
            
            moment = atof((char*)buf+2);            //Fjern udkommentering når det kopieres over
			update(NULL, &moment, 0);
        }
        else if(buf[0]=='5') // PID regulations
        {
            int i = 0;
            uint8 temp = 0;
            float PID[3];
            //buf[buf_n+1] = 0;
            temp = atoi(strtok((char *)buf," "));
            for(i = 0; i<3 ;i++)
            {
                PID[i] = atof(strtok(NULL, " "));//eventuelt bare NULL istedet for (char *)buf
            }
            struct PIDparameter PIDval = *getPID_ptr();
            
            PIDval.Kp = PID[0]; PIDval.Ki = PID[1]; PIDval.Kd = PID[2]; PIDval.valid = 1;
            
            update(&PIDval, NULL, 0);
            
        }
        else if(buf[0]=='6') // calibrate
        {
            calibrate();//skal fjernes
            buf[buf_n+1]=0;
            
//            if(strcmp((char*)buf, "6\n")==0)
//            {
//               calibrate();
//            }
        }
        else if(buf[0]=='7') // reset
        {
            update(NULL,NULL,1);
            buf[buf_n+1]=0;
        }
        
    }
}

void SendUART (char *Pdata)
{

    while(USBUART_1_CDCIsReady() == 0u);
    USBUART_1_PutString((char*)Pdata);
    return;
}

void SendData (struct data* Data,  float setForce, float PIDinput, float PIDsensor, float *PIDdebug)
{
    if(isReadyToSend == 0)
        return;
    
    CheckConnection();
    
    if (USBUART_1_GetConfiguration() == 0u)
        return;
        
    
    if( USBUART_1_DataIsReady() != 0)
        return;

    
    char buf[500];
       
    sprintf(buf, "3 %lu %f %f %f %f %lu %f %f %f %d %f %f %f %f %f %f %f %f %f\n\r", 
        Data->time_ms, 
        MomentToForce(Data->Moment.avg), 
        Data->V_motor.avg, 
        Data->A_motor.avg, 
        Data->P_motor.avg,
        Data->distance, 
        RPMToSpeed(Data->RPM.avg), 
        Data->P_mekanisk.avg, 
        Data->efficiency.avg,
        Data->stop,
        setForce,
        PIDdebug[0],//pidval
        PIDdebug[1], //err
        PIDdebug[2],//antiwindup
        PIDinput, //input
        PIDsensor, //sensor
        MomentToForce(Data->Moment.rms), 
        Data->V_motor.rms, 
        Data->A_motor.rms
    );
    SendUART(buf);
    /*
     SendUART("1 10 Force_set (N)\n");
                CyDelay(1);
                SendUART("1 11 PIDval\n");
                CyDelay(1);
                SendUART("1 12 PIDerr\n");
                CyDelay(1);
                SendUART("1 13 PIDanti_windup\n");
                CyDelay(1);
                SendUART("1 14 PIDinput\n");
                CyDelay(1);
                SendUART("1 15 PIDsensor\n");
                CyDelay(1);
    */
}

/* [] END OF FILE */
