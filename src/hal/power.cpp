#include "hal.h"
#include <Arduino.h>
#include <EEPROM.h>
#include "EEPROM_utils.h"

void HAL::power_init(void)
{
    /*初始化EEPROM*/
    EEPROM.begin(4096);

    // 首次使用初始化EEPROM
    if (EEPROM.read(4095) != 1)
    {
        clean_EEPROM();
        EEPROM.write(4095, 1);
        EEPROM.commit();
    }
    
}

int32_t HAL::LightMode(void)
{
    return EEPROM.read(SYSTEM_ADDR);
}

void HAL::power_off(void)
{
    if (HAL::LightMode() == 1)
    {

        EEPROM.write(SYSTEM_ADDR, 0);
        EEPROM.commit();
        vTaskDelay(20);
        Serial.print("HAL::LightMode(): ");
        Serial.println(HAL::LightMode());
        ESP.restart();
    }
    else
    {
        EEPROM.write(SYSTEM_ADDR, 1);
        EEPROM.commit();
        vTaskDelay(20);
        Serial.print("HAL::LightMode(): ");
        Serial.println(HAL::LightMode());
        ESP.restart();
    }
}