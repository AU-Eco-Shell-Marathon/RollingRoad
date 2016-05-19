#include <project.h>
#include "Uart.h"
#include "ControllerClass.h"
#include "PID.h"
#include "sensor.h"
#include "Constants.h"
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
                SendUART("1 1 Effect(EL) W\n");
                CyDelay(1);
                SendUART("1 2 Effect(MEK) W\n");
                CyDelay(1);
                SendUART("1 3 efficiency procent\n");
                CyDelay(1);
                SendUART("1 4 set_Torque N\n");
                CyDelay(1);
                SendUART("1 5 set_Force N\n");
                CyDelay(1);
                SendUART("1 6 Force N\n");
                CyDelay(1);
                SendUART("1 7 Moment Nm\n");
                CyDelay(1);
                SendUART("1 8 distance m\n");
                CyDelay(1);
                SendUART("1 9 Speed m/s\n");
                CyDelay(1);
                SendUART("1 10 Rotation RPM\n");
                CyDelay(1);
                SendUART("1 11 V_Motor V\n");
                CyDelay(1);
                SendUART("1 12 A_Motor A\n");
                CyDelay(1);
                SendUART("1 13 Error bool\n");
                CyDelay(1);
                #if TEST == 1
                SendUART("1 14 PID_value NAn\n");
                CyDelay(1);
                SendUART("1 15 PID_error NAn\n");
                CyDelay(1);
                SendUART("1 16 PID_antiwindup NAn\n");
                CyDelay(1);
                SendUART("1 17 PID_input NAn\n");
                CyDelay(1);
                SendUART("1 18 PID_sensor NAn\n");
                CyDelay(1);
                SendUART("1 19 Alpha_value_uint16 NAn\n");
                CyDelay(1);
                #endif
                char buf[50];
                sprintf(buf, "5 %f %f %f\n",
                    PIDval.Kp,
                    PIDval.Ki,
                    PIDval.Kd
                );
                SendUART(buf);
                
                isReadyToSend = 1;
               // CyDelay(100);    HUSK AT OPDATER MED SENESTE PROTOKOL
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
        else if(buf[0]=='4') //modtag torque fra PC
        {
            
            float torque = 0;
            uint8 i;
            
            
            
            for(i = 0; i< 250; i++)
            {
                if(buf[i] == '\n' || buf[i] == '\r' || buf[i] == 0)
                {
                    buf[i] = 0;
                    break;
                }
            }
            
            torque = atof((char*)buf+2);
			update(NULL, &torque, 0);
        }
        else if(buf[0]=='5') // PID regulations
        {
            int i = 0;
            float PID[3];
            
            atoi(strtok((char *)buf," "));
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
        else if(buf[0]=='7') //modtag Force fra PC
        {
            
            float torque = 0;
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
            
            torque = atof((char*)buf+2) * ForceToTorque;            //Fjern udkommentering når det kopieres over
			update(NULL, &torque, 0);
        }
        
    }
}

void SendUART (char *Pdata)
{

    while(USBUART_1_CDCIsReady() == 0u);
    USBUART_1_PutString((char*)Pdata);
    return;
}

void SendData (struct data* Data,  float set_torque, float *PIDdebug)
{
    if(isReadyToSend == 0)
        return;
    
    CheckConnection();
    
    if (USBUART_1_GetConfiguration() == 0u)
        return;
        
    
    if( USBUART_1_DataIsReady() != 0)
        return;

    
    char buf[500];
    
    #if TEST == 0
    sprintf(buf, "3 %lu %f %f %f %f %f %f %f %lu %f %f %f %f %u\n\r", 
        Data->time_ms,              //Tid (ms)
        Data->P_motor,              //effect (P)
        Data->P_mekanisk,           
        Data->efficiency,           
        set_torque,                   //set Force (N)
        TorqueToForce*set_torque,                   //set Force (N)
        TorqueToForce*Data->Moment,//Force (N)
        Data->Moment,               //Moment (Nm)
        Data->distance, 
        RPM_To_Speed*Data->RPM,      //Moment (m/s)
        Data->RPM,
        Data->V_motor,
        Data->A_motor, 
        Data->stop
    );
    #else
    sprintf(buf, "3 %lu %f %f %f %f %f %f %f %lu %f %f %f %f %u %f %f %f %f %f %lu\n\r", 
        Data->time_ms,              //Tid (ms)
        Data->P_motor,              //effect (P)
        Data->P_mekanisk,           
        Data->efficiency,           
        set_torque,                   //set Force (N)
        TorqueToForce*set_torque,                   //set Force (N)
        TorqueToForce(Data->Moment),//Force (N)
        Data->Moment,               //Moment (Nm)
        Data->distance, 
        RPMToSpeed(Data->RPM),      //Moment (m/s)
        Data->RPM,
        Data->V_motor,
        Data->A_motor, 
        Data->stop,
        PIDdebug[0],//pidval
        PIDdebug[1], //err
        PIDdebug[2],//antiwindup
        PIDinput, //input
        PIDsensor, //sensor
        (uint32)Data->Alpha
    );
    #endif
    SendUART(buf);
}

/* [] END OF FILE */
