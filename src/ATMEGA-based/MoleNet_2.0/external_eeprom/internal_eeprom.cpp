#include "internal_eeprom.h"

InternalEEPROM::InternalEEPROM(ExternalEEPROM *_ext) : 
    externalEEPROM(_ext) 
{ 
}
    
void InternalEEPROM::saveStartupConfig()
{
    for (uint8_t i = 0; i < StartupConfig::DATA_SIZE; ++i)
        EEPROM.write(i, config[i]);
    
    uint32_t addr = externalEEPROM->getNextAddress();
    EEPROM.write(StartupConfig::DATA_SIZE+0, (addr >> 24) & 0xFF);  
    EEPROM.write(StartupConfig::DATA_SIZE+1, (addr >> 16) & 0xFF);  
    EEPROM.write(StartupConfig::DATA_SIZE+2, (addr >> 8) & 0xFF);  
    EEPROM.write(StartupConfig::DATA_SIZE+3, (addr ) & 0xFF);  
}    

void InternalEEPROM::loadStartupConfig()
{
    for (uint8_t i = 0; i < StartupConfig::DATA_SIZE; ++i)
        config[i] = EEPROM.read(i);
    
    externalEEPROM->setNextAddress( (uint32_t) EEPROM.read(StartupConfig::DATA_SIZE+0) << 24 |
                                    (uint32_t) EEPROM.read(StartupConfig::DATA_SIZE+1) << 16 |
                                    (uint32_t) EEPROM.read(StartupConfig::DATA_SIZE+2) << 8 |
                                    (uint32_t) EEPROM.read(StartupConfig::DATA_SIZE+3) );
}