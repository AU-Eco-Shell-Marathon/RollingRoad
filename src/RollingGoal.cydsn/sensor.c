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
#include "sensor.h"
#include <Math.h>
#include "Constants.h"

//prototypes
CY_ISR_PROTO(SAR_ADC_1);
CY_ISR_PROTO(SAR_ADC_2);
CY_ISR_PROTO(RPM_isr);
void DMA_setup_DelSig();

//global variables

uint16  Alpha_ = 0x00FF;
int32   y_V_motor = 0,
        y_A_motor = 0,
        y_Moment = 0;
uint32  y_RPM = 0;

int32   V_motor_offset = 0,
        A_motor_offset = 0,
        Moment_offset  = 0;

int16   Moment_temp = 0;
uint32  RPM_temp = 0;


// DMA setup ----------
// DelSig
#define DMA_DelSig_eoc_BYTES_PER_BURST 2
#define DMA_DelSig_eoc_REQUEST_PER_BURST 1
#define DMA_DelSig_eoc_SRC_BASE (CYDEV_PERIPH_BASE)
#define DMA_DelSig_eoc_DST_BASE (CYDEV_PERIPH_BASE)

uint8 DMA_DelSig_eoc_Chan;
uint8 DMA_DelSig_eoc_TD[1];
//DFB
#define DMA_Filter_BYTES_PER_BURST 4
#define DMA_Filter_REQUEST_PER_BURST 1
#define DMA_Filter_SRC_BASE (CYDEV_PERIPH_BASE)
#define DMA_Filter_DST_BASE (CYDEV_SRAM_BASE)

uint8 DMA_Filter_Chan;
uint8 DMA_Filter_TD[1];

void DMA_setup_DelSig()
{
    
    //DelSig
    ADC_DelSig_1_Start();
    ADC_DelSig_1_SetCoherency(ADC_DelSig_1_COHER_MID);
    ADC_DelSig_1_StartConvert();

    
    DMA_DelSig_eoc_Chan = DMA_DelSig_eoc_DmaInitialize(DMA_DelSig_eoc_BYTES_PER_BURST, DMA_DelSig_eoc_REQUEST_PER_BURST, 
    HI16(DMA_DelSig_eoc_SRC_BASE), HI16(DMA_DelSig_eoc_DST_BASE));
    DMA_DelSig_eoc_TD[0] = CyDmaTdAllocate();
    CyDmaTdSetConfiguration(DMA_DelSig_eoc_TD[0], 2, DMA_INVALID_TD, TD_INC_DST_ADR);
    CyDmaTdSetAddress(DMA_DelSig_eoc_TD[0], LO16((uint32)ADC_DelSig_1_DEC_SAMP_PTR), LO16((uint32)Filter_1_STAGEAM_PTR));
    CyDmaChSetInitialTd(DMA_DelSig_eoc_Chan, DMA_DelSig_eoc_TD[0]);
    CyDmaChEnable(DMA_DelSig_eoc_Chan, 1);
    
    //DFB
    Filter_1_Start();
    Filter_1_SetCoherency(Filter_1_CHANNEL_A, Filter_1_KEY_HIGH);
    
    DMA_Filter_Chan = DMA_Filter_DmaInitialize(DMA_Filter_BYTES_PER_BURST, DMA_Filter_REQUEST_PER_BURST, 
        HI16(DMA_Filter_SRC_BASE), HI16(DMA_Filter_DST_BASE));
    DMA_Filter_TD[0] = CyDmaTdAllocate();
    CyDmaTdSetConfiguration(DMA_Filter_TD[0], 2, DMA_INVALID_TD, DMA_Filter__TD_TERMOUT_EN | TD_INC_SRC_ADR);
    CyDmaTdSetAddress(DMA_Filter_TD[0], LO16((uint32)Filter_1_HOLDAM_PTR), LO16((uint32)&Moment_temp));
    CyDmaChSetInitialTd(DMA_Filter_Chan, DMA_Filter_TD[0]);
    CyDmaChEnable(DMA_Filter_Chan, 1);
    
}


CY_ISR(SAR_ADC_1)
{
    y_V_motor = (int32)(((int32)Alpha_*(int32)ADC_SAR_Seq_1_GetResult16(0) + (int32)(65536u-Alpha_)*y_V_motor)>>16);
    y_A_motor = (int32)(((int32)Alpha_*(int32)ADC_SAR_Seq_1_GetResult16(1) + (int32)(65536u-Alpha_)*y_A_motor)>>16);
    y_Moment = (int32)(((int32)Alpha_*(int32)Moment_temp + (int32)(65536u-Alpha_)*y_Moment)>>16);
    y_RPM = (uint32)(((uint64)Alpha_*(uint64)RPM_temp + (uint64)(65536u-Alpha_)*(uint64)y_RPM)>>16);
}

char RPM_reset=0;
CY_ISR(RPM_isr)
{
    
    uint8 STATUS = Timer_1_STATUS;
    
    if((STATUS&(1<<1)))//capture
    {
        uint32 temp = Timer_1_ReadCapture();
        uint32 temp2 = 0;
        if(Timer_1_STATUS&(1<<3))
            temp2 = Timer_1_ReadCapture();
            
        
        RPM_reset=0;
        
        if(temp2 != 0)
        {
            //RPM_temp = 100000000/(temp - temp2);
            RPM_temp = (16666667)/(temp - temp2); // 100000000*60/360=16666667
            //(277778*60)/(temp - temp2);
        }
        else
        {
            RPM_temp=0;
        }
        
    }
    else if((STATUS&(1<<0)))//TC
    {
        
        if(RPM_reset)
        {
            RPM_temp=0;
        }
        RPM_reset=1;
        
    }
    Timer_1_WriteCounter(200000);
    Timer_1_ClearFIFO();
}

void sensor_init(int32 VM, int32 AM, int32 moment)
{
    DMA_setup_DelSig();
    
    V_motor_offset = VM;
    A_motor_offset = AM;
    Moment_offset = moment;
    
    isr_1_StartEx(SAR_ADC_1);
    isr_3_StartEx(RPM_isr);
    
    ADC_SAR_Seq_1_Start();
    
    ADC_SAR_Seq_1_StartConvert();
   
    
    
    Timer_1_Start();
    Counter_1_Start();
    
    Counter_2_Start();
    
    Opamp_1_Start();
    VDAC8_1_Start();
    Comp_1_Start();
    
    Clock_1_Start();
    Clock_2_Start();
    Clock_3_Start();
    Clock_5_Start();
    
    Control_Reg_1_Write(0b1);
    
}

void sensor_calibrate(int32* VM, int32* AM, int32* moment)
{
    V_motor_offset = 0;
    A_motor_offset = 0;
    Moment_offset = 0;
    
    uint16 Alpha_temp = Alpha_;
    Alpha_ = 0x000F;
    
    uint32 CurrentTime = Counter_2_ReadCounter();
    
    while(Counter_2_ReadCounter() < CurrentTime + 5000);
    
    V_motor_offset = y_V_motor;
    A_motor_offset = y_A_motor;
    Moment_offset = y_Moment;
    
    Alpha_ = Alpha_temp;
    
    *VM = V_motor_offset;
    *AM = A_motor_offset;
    *moment = Moment_offset;
    
}

char getData(struct data * Data)
{
 
    Data->A_motor       =  ADC_SAR_Seq_1_CountsTo_Volts(y_A_motor - A_motor_offset)*VoltToCurrent;
    Data->V_motor       =  ADC_SAR_Seq_1_CountsTo_Volts(y_V_motor - V_motor_offset)*VoltToVolt;
    Data->Moment        =  (ADC_DelSig_1_CountsTo_Volts(y_Moment - Moment_offset) > 0.0f ? ADC_DelSig_1_CountsTo_Volts(y_Moment - Moment_offset) : -ADC_DelSig_1_CountsTo_Volts(y_Moment - Moment_offset))*VoltToTorque;
    Data->RPM           =  (float)y_RPM / 1000.0f;
    
    Data->P_mekanisk    = Data->Moment * Data->RPM * RPM_Nm_To_W;
    Data->P_motor       = Data->A_motor * Data->V_motor;
    
    
    
    Data->efficiency    = (Data->P_motor != 0.0f ? (Data->P_mekanisk / Data->P_motor) * 100.0f : 0.0f);
    
    Data->distance      = (uint32)((2.0f*PI*RR_radius*(float)Counter_1_ReadCounter())/360.0f);
    Data->time_ms       = Counter_2_ReadCounter();
    Data->stop          = Status_Reg_1_Read()&0b1;
    Data->Alpha         = Alpha_;
    
    return 1;
}

float getMoment()
{
    return (ADC_DelSig_1_CountsTo_Volts(y_Moment - Moment_offset) > 0.0f ? ADC_DelSig_1_CountsTo_Volts(y_Moment - Moment_offset) : -ADC_DelSig_1_CountsTo_Volts(y_Moment - Moment_offset))*VoltToTorque;
}

float getRPM()
{
    return (float)y_RPM / 1000.0f;
}

int32 getDistance(char reset)
{
    if(reset)
    {
        Control_Reg_2_Write(0b1);
    }
    return Counter_1_ReadCounter()*((2.0f*RR_radius*PI)/360.0f);
}


/* [] END OF FILE */
