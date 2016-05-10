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

#ifndef SENSOR_H
#define SENSOR_H
    
#include <project.h>



// Alt data kommer i micro prefix, undtagen RPM og distance.

struct sample
{
    float avg;
    float rms;
    float min;
    float max;
};

struct data
{
    struct sample V_motor;
    struct sample A_motor;
    struct sample P_motor;
    struct sample RPM;
    struct sample Moment;
    struct sample P_mekanisk;
    struct sample efficiency;
    uint32 distance;
    uint32 time_ms;
    char stop;
};


char getData(struct data *);
float getMoment();
int32 getDistance(char reset);

void setSNRdB(float SNRdB);

void sensor_init(int32 VM, int32 AM, int32 moment, int32 AG);
void sensor_calibrate(int32* VM, int32* AM, int32* moment, int32* AG);
float MomentToForce(float Moment_value);
float ForceToMoment(float Force_value);
float RPMToSpeed(float RPM_value);


#endif

/* DMA kode
// Defines for DMA_DelSig_eoc
#define DMA_DelSig_eoc_BYTES_PER_BURST 2
#define DMA_DelSig_eoc_REQUEST_PER_BURST 1
#define DMA_DelSig_eoc_SRC_BASE (CYDEV_PERIPH_BASE)
#define DMA_DelSig_eoc_DST_BASE (CYDEV_PERIPH_BASE)

//Variable declarations for DMA_DelSig_eoc 
//Move these variable declarations to the top of the function
uint8 DMA_DelSig_eoc_Chan;
uint8 DMA_DelSig_eoc_TD[1];

// DMA Configuration for DMA_DelSig_eoc
DMA_DelSig_eoc_Chan = DMA_DelSig_eoc_DmaInitialize(DMA_DelSig_eoc_BYTES_PER_BURST, DMA_DelSig_eoc_REQUEST_PER_BURST, 
    HI16(DMA_DelSig_eoc_SRC_BASE), HI16(DMA_DelSig_eoc_DST_BASE));
DMA_DelSig_eoc_TD[0] = CyDmaTdAllocate();
CyDmaTdSetConfiguration(DMA_DelSig_eoc_TD[0], 2, CY_DMA_DISABLE_TD, 0);
CyDmaTdSetAddress(DMA_DelSig_eoc_TD[0], LO16((uint32)ADC_DelSig_1_DEC_SAMP_PTR), LO16((uint32)Filter_1_STAGEAH_PTR));
CyDmaChSetInitialTd(DMA_DelSig_eoc_Chan, DMA_DelSig_eoc_TD[0]);
CyDmaChEnable(DMA_DelSig_eoc_Chan, 1);

*/


/* [] END OF FILE */

