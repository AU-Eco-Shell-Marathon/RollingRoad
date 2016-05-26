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
#include <project.h>
#include "ControllerClass.h"

int main()
{

    ControllerClass_init();
    
    for(;;)
    {
        ControllerClass_run();
    }
}

/* [] END OF FILE */
