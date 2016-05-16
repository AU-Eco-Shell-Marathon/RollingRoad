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
#define N 128u

// help functions

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
