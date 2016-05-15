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




#define N 128u

CY_ISR_PROTO(SAR_ADC_1);
CY_ISR_PROTO(SAR_ADC_2);
CY_ISR_PROTO(RPM_isr);

void calcSamples(const int32 * values, const uint16 N_sample, struct sample * Sample, int32 divider);
void convertToUnit(int32 * value, const uint16 N_sample,int32 (*CountsTo)(int16), const uint8 type);
int32 CountToAmp(int32);
int32 CountToMoment(int32);
void DMA_setup_DelSig();

static char calibrate_ = 0;

float maxEfficiencyRMS = 1.0; 
uint16 Alpha_ = 0x0FFF;
int32 y_V_motor = 0,
y_A_motor = 0,
y_Moment = 0,
y_RPM = 0;


// Mållinger.
int32 V_motor[N];   int32 V_motor_offset = 0;
int32 A_motor[N];   int32 A_motor_offset = 0;
int32 Moment[N];    int32 Moment_offset  = 0;
int32 RPM[N];
volatile uint16 n = 0;

int16 Moment_temp = 0;
uint32 RPM_Moment_temp = 0;

// Mållinger af RPM

uint32 RPM_temp = 0;

// strømovervågning til belastningskredsløb.
int16 A_generator_max = 0;
int32 A_generator_sum = 0;
int16 A_generator_min = 0xEFFF;
uint16 A_generator_samples = 0;

int32 A_generator_offset = 0;

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
    RPM_Moment_temp = RPM_temp;
    y_V_motor = (int32)(((int32)Alpha_*ADC_SAR_Seq_1_GetResult16(0) + (int32)(65536u-Alpha_)*y_V_motor)>>16);
    y_A_motor = (int32)(((int32)Alpha_*ADC_SAR_Seq_1_GetResult16(1) + (int32)(65536u-Alpha_)*y_A_motor)>>16);
    y_Moment = (int32)(((int32)Alpha_*Moment_temp + (int32)(65536u-Alpha_)*y_Moment)>>16);
    y_RPM = (int32)(((int32)Alpha_*RPM_Moment_temp + (int32)(65536u-Alpha_)*y_RPM)>>16);
    
    if(n == N)
        return;
    
    //V_motor[n]=(int32)ADC_SAR_Seq_1_GetResult16(0) - V_motor_offset;
    //A_motor[n]=(int32)ADC_SAR_Seq_1_GetResult16(1) - A_motor_offset;
    
    V_motor[n]=y_V_motor - V_motor_offset;
    A_motor[n]=y_A_motor - A_motor_offset;
    
    
    //Moment[n]=Moment_temp - Moment_offset;
    Moment[n]=y_Moment - Moment_offset;
    RPM[n]=RPM_Moment_temp;
    
    n++;
}

CY_ISR(SAR_ADC_2)
{
    if(A_generator_samples == 0xFFFF)
        return;
    
    
    int16 A_temp = ADC_SAR_1_GetResult16() - A_generator_offset;
    
    if(A_temp > A_generator_max)
        A_generator_max = A_temp;
    else if(A_temp < A_generator_min)
        A_generator_min = A_temp;
    
    A_generator_sum += A_temp;
    
    A_generator_samples++;
    
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

void sensor_init(int32 VM, int32 AM, int32 moment, int32 AG)
{
    setSNRdB(50.0f);
    DMA_setup_DelSig();
    
    V_motor_offset = VM;
    A_motor_offset = AM;
    Moment_offset = moment;
    A_generator_offset = AG;
    
    isr_1_StartEx(SAR_ADC_1);
    isr_2_StartEx(SAR_ADC_2);
    isr_3_StartEx(RPM_isr);
    
    ADC_SAR_1_Start();
    ADC_SAR_Seq_1_Start();
    
    ADC_SAR_1_StartConvert();
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



void sensor_calibrate(int32* VM, int32* AM, int32* moment, int32* AG)
{
    calibrate_ = 1;
    V_motor_offset = 0;
    A_motor_offset = 0;
    Moment_offset = 0;
    A_generator_offset = 0;
    n = 0;
    
    
    //n = 0;
    while(n != N)
    {
        __asm__ __volatile__("nop;"); // for tvinge den til at blive ved med at tjekke om n != N! fejlen kommer nok af at før while bliver n sat til 0.
    }
    uint16 i;
    
    int32 AVG[4] = {V_motor[0],A_motor[0],Moment[0], A_generator_sum/A_generator_samples};
    //int32 MAX[3] = {V_Motor[0],A_motor[0],Moment[0]};
    //int32 MIN[3] = {V_Motor[0],A_motor[0],Moment[0]};
    
    for(i = 1; i<N; i++)
    {
        AVG[0] += V_motor[i];
        AVG[1] += A_motor[i];
        AVG[2] += Moment[i];
    }
    
    AVG[0] = AVG[0]/N;
    AVG[1] = AVG[1]/N;
    AVG[2] = AVG[2]/N;
    
    V_motor_offset = AVG[0];
    A_motor_offset = AVG[1];
    Moment_offset = AVG[2];
    A_generator_offset = AVG[3];
    
    *VM = AVG[0];
    *AM = AVG[1];
    *moment = AVG[2];
    *AG = AVG[3];
    
    n = 0;
    calibrate_ = 0;
}


char getData(struct data * Data)
{

    if(n != N || calibrate_)
        return 0;
    
    convertToUnit(V_motor, n, &ADC_SAR_Seq_1_CountsTo_uVolts,0);
    convertToUnit(A_motor, n, &ADC_SAR_Seq_1_CountsTo_uVolts,1);
    convertToUnit(Moment, n, NULL,3);
    
    int32 P_motor[N];
    int32 P_mekanisk[N];
    int32 efficiency[N];
    uint16 i;
    for(i = 0; i < N; i++)
    {
        P_motor[i] = (int32)(((int64)V_motor[i]*(int64)A_motor[i])/1000000);  //Brug af shift istedet vil koste 4.63% // uWatt
        P_mekanisk[i] = (int32)(((((int64)Moment[i]*(int64)RPM[i])/1000000) * ((int64)RPM_Nm_To_uW))/1000); //uWatt
        efficiency[i] = (int32)(((int64)P_mekanisk[i]*1000)/P_motor[i]) ; //promille
    }

    calcSamples(V_motor, n, &Data->V_motor, 1000000);
    calcSamples(A_motor, n, &Data->A_motor, 1000000);
    calcSamples(Moment, n, &Data->Moment, 1000000);
    calcSamples(RPM, n, &Data->RPM, 1000);
    
    calcSamples(P_motor, n, &Data->P_motor, 1000000);
    calcSamples(P_mekanisk, n, &Data->P_mekanisk, 1000000);
    
    calcSamples(efficiency, n, &Data->efficiency, 10); // udprinter i procent.
    
    /* snyde koder ;)
    Data->P_motor.avg = Data->V_motor.avg*Data->A_motor.avg;
    Data->P_mekanisk.avg = Data->Moment.avg*Data->RPM.avg*0.10472f;
    Data->efficiency.avg = (Data->P_mekanisk.avg*100)/Data->P_motor.avg;
    */
    Data->distance = (uint32)((2.0f*PI*RR_radius*(float)Counter_1_ReadCounter())/360.0f);
    Data->time_ms = Counter_2_ReadCounter();
    Data->stop = Status_Reg_1_Read()&0b1;
    Data->Alpha = Alpha_;
    Data->maxRMS = maxEfficiencyRMS;
    //skal ændres
    /*
    if(Data->efficiency.rms > maxEfficiencyRMS)
    {
        Alpha_= (Alpha_ != 0x0001 ? (Alpha_-1) : (Alpha_));
    }
    else
    {
        Alpha_= (Alpha_ != 0xFFFE ? (Alpha_+1) : (Alpha_));
    }
    */
    n=0;
    return 1;
    
}

void setSNRdB(float SNRdB)
{
    maxEfficiencyRMS = 0.25e2 * sqrt(0.2e1) / exp(SNRdB * log(0.10e2) / 0.20e2);
}


float getMoment()
{
    //CountToMoment(ADC_DelSig_1_CountsTo_uVolts(value[i]))
    
    float temp = ADC_DelSig_1_CountsTo_Volts(Moment_temp - Moment_offset)*2;
    if(temp<0)
        return -temp;
    else
        return temp;
}

int32 getDistance(char reset)
{
    if(reset)
    {
        Control_Reg_2_Write(0b1);
    }
    return Counter_1_ReadCounter()*((2.0f*RR_radius*PI)/360.0f);
}

void calcSamples(const int32 * values,const uint16 N_sample, struct sample * Sample,int32 divider)
{
    int64 AVG = values[0];
    int32 MAX = values[0];
    int32 MIN = values[0];

    Sample->rms = 0;

    uint16 i;
    for(i = 1; i < N_sample; i++)
    {         
        AVG += values[i];
        
        if(values[i] > MAX)
            MAX = values[i];
        else if(values[i] < MIN)
            MIN = values[i];

    }
    
    if(N_sample == 128)
        AVG = AVG>>7;
    else if(N_sample == 512)
        AVG = AVG>>9;
    else
        AVG = AVG/N_sample;
    
    for(i = 0; i < N_sample; i++)
    {

        Sample->rms += ((int64)values[i] - AVG)^2;

    }
    Sample->rms = Sample->rms/N_sample;
    if(Sample->rms < 0)
        Sample->rms = 0;
    else
        Sample->rms = sqrt((double)Sample->rms)/divider;
    
    Sample->avg = (float)AVG/divider;
    Sample->max = (float)MAX/divider;
    Sample->min = (float)MIN/divider;
}

void convertToUnit(int32 * value, const uint16 N_sample,int32 (*CountsTo)(int16), const uint8 type)
{
    uint8 i;
    for(i = 0; i < N_sample; i++)
    {
        if(type == 0)
            value[i] = CountsTo(value[i])*11;
        else if(type == 1)
            value[i] = CountToAmp(CountsTo(value[i]));
        else if(type == 2)
            value[i] = CountToMoment(CountsTo(value[i]));
        else if(type == 3)
            value[i] = CountToMoment(ADC_DelSig_1_CountsTo_uVolts(value[i]));
    }
    
}

int32 CountToMoment(int32 uvolt)
{
    if(uvolt < 0)
        return -uvolt*2;
    else
        return uvolt*2;
}

int32 CountToAmp(int32 uvolt)
{
    return uvolt*24;
}

float MomentToForce(float Moment_value)
{
    return Moment_value * RR_radius_reciprocal;
}

float ForceToMoment(float Force_value)
{
    return Force_value * RR_radius;
}

float RPMToSpeed(float RPM_value)
{
    return RPM_value * RPM_To_Speed;
}

/* [] END OF FILE */
