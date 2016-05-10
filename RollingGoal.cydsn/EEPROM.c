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
#include "EEPROM.h"
#define START_EEPROM_SECTOR  (1u)
#define START_BYTE         ((START_EEPROM_SECTOR * EEPROM_SIZEOF_SECTOR) + 0x00)


size_t dataSize[255] = {0};


char EEPROM_read(uint8 id, uint8 * data)
{
    uint32 i;
    uint32 pos = 0;
    for(i = 0; i < id; i++)
    {
        pos += dataSize[i] + 1;
    }
    
    if(EEPROM_1_ReadByte(START_BYTE + pos + dataSize[id]) != 0x33)
    {
        return 0; // ikke noget gemt data
    }
    
    for(i = 0; i < dataSize[id]; i++)
    {
        data[i] = EEPROM_1_ReadByte(START_BYTE + pos + i);
    }
    
    return 1;
}


char EEPROM_write(uint8 id, const uint8 * data)
{
    uint32 i;
    uint32 pos = 0;
    for(i = 0; i < id; i++)
    {
        pos += dataSize[i] + 1;
    }
    
    if(dataSize[id] == 0)
        return -1;
    
    for(i = 0; i < dataSize[id]; i++)
    {
        if(EEPROM_1_WriteByte((uint8)data[i], START_BYTE + pos + i) != CYRET_SUCCESS)
        {
            return -1; //fejl i tilskrivning
        }
    }
    
    if(EEPROM_1_WriteByte(0x33, START_BYTE + pos + dataSize[id]) != CYRET_SUCCESS)
    {
        return -1; //fejl i tilskrivning
    }
    
    return 1;
}

char EEPROM_init(const size_t * types, uint8 count)
{
    EEPROM_1_Start();
    uint8 i;
    
    uint32 size = 0;
    
    for(i = 0; i < count; i++)
    {
        dataSize[i] = types[i];   
        size += types[i];
    }
    
    if(size > EEPROM_SIZEOF_SECTOR)
    {
        return -1;   //EEPROM er ikke stor nok.
    }
    
    
    
    return 1;
}



/* [] END OF FILE */
