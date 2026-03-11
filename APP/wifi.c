/*
 * wifi.c
 *
 *  Created on: Mar 18, 2024
 *      Author: yyz
 *  Modified for A2610: PA6 as WiFi RST
 */

#include "HAL.h"
#include "wifi.h"
#include "running.h"
#include "parameter.h"

void wifi_init(void)
{
    // A2610 WiFi 复位引脚: PA6
    GPIOA_ResetBits(GPIO_Pin_6);  // WiFi 复位
    GPIOA_ModeCfg(GPIO_Pin_6, GPIO_ModeOut_PP_20mA);

    wifiOnoff(1);
}

void wifiOnoff(uint8_t state)
{
    if(state == 1)
    {
        GPIOA_SetBits(GPIO_Pin_6);  // WiFi 使能
    }else if(state == 0)
    {
        GPIOA_ResetBits(GPIO_Pin_6);  // WiFi 复位
    }
}
