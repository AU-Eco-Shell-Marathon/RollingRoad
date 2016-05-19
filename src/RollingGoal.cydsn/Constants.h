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
#ifndef CONSTANTS_H
    #define CONSTANTS_H

#define TEST 0 
    
    
#define PI 3.141592654f
#define RR_radius 0.076f
#define RR_radius_reciprocal 13.15789474f
#define gear 4.3f
#define RPM_Nm_To_uW 104720 //((2*pi)/60)*10^6
#define RPM_Nm_To_W 0.104720f
#define RPM_To_Speed 0.0079587f // m/s
#define VoltToVolt 11
#define VoltToCurrent 24
#define VoltToTorque 2
#define TorqueToForce RR_radius_reciprocal
#define ForceToTorque RR_radius
    // PID //
#define P_RPM_reciprocal 188.7451117f
#define Kp_def 1035.1f;
#define Ki_def 81.6327f;
#define Kd_def 0.0f;
#define MAX_def 255.0f;
#define MIN_def 0.0f;
#define dt_def 0.0005f;    
#endif
/* [] END OF FILE */
