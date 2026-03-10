/*
 * EV3P035J.c
 *
 *  Created on: 2023年6月16日
 *      Author: wx030
 */
#include "EV3P035J.h"
#include "HAL.h"
ring_buffer rb1;
uint8_t rb1_buff[8];
/*****************************************************************************
 [函数名称]vEV3P035JInit
 [函数功能]EV3P035J初始化
 [参    数]
 [全局变量]无
 [返 回 值]无
 [备    注]
 *****************************************************************************/
void vEV3P035JInit(void)
{
    //GPIOB_ModeCfg(GPIO_Pin_5 , GPIO_ModeIN_PU);
    //GPIOB_ModeCfg(GPIO_Pin_6 , GPIO_ModeIN_PU);
    GPIOB_SetBits(GPIO_Pin_5);
    GPIOB_SetBits(GPIO_Pin_6);
    GPIOB_ModeCfg(GPIO_Pin_5 , GPIO_ModeOut_PP_5mA);
    GPIOB_ModeCfg(GPIO_Pin_6 , GPIO_ModeOut_PP_5mA);
    GPIOB_ResetBits(GPIO_Pin_5);
    GPIOB_ResetBits(GPIO_Pin_6);
//    RB_Init(&rb1,rb1_buff,8);
}

/*****************************************************************************
 [函数名称]vEV3P035JPlay()
 [函数功能]EV3P035J播放音频
 [参    数]num 音频序号
 [全局变量]无
 [返 回 值]无
 [备    注]
 *****************************************************************************/
void vEV3P035JPlay(uint8_t num)
{
#if GERMAN==0
#elif GERMAN==1
    if(num>THANKS)  return;
#endif
    if(baseParameters.languageType != 0)
    {
#if GERMAN==0
        if(num <= THANKS)
        {
            num = num + 12 * (baseParameters.languageType-1);

        }else if(num>=LEVEL_MODE && num<=TEMP_MODE)
        {
            num = num + 2 * (baseParameters.languageType-1);
        }else if(num >= WATER_SENSOR_FAIL)
        {
            switch(baseParameters.languageType)
            {
            case SPEECHLANGUARE_ENGLISH:
                num = num + 3;
                break;
            case SPEECHLANGUARE_CHINESE:
                break;
            case SPEECHLANGUARE_RUSSIAN:
                num = num + 6;
                break;
            }
        }
#elif GERMAN==1
        if(num <= THANKS && num!=0)
        {
            num = num + 12 * (baseParameters.languageType-1);
        }
#endif
//        else if(num>=LEVEL_MODE && num<=TEMP_MODE)
//        {
//            num = num + 2 * (baseParameters.languageType-1);
//        }else if(num >= WATER_SENSOR_FAIL)
//        {
//            switch(baseParameters.languageType)
//            {
//            case SPEECHLANGUARE_ENGLISH:
//                num = num + 3;
//                break;
//            case SPEECHLANGUARE_CHINESE:
//                break;
//            case SPEECHLANGUARE_RUSSIAN:
//                num = num + 6;
//                break;
//            }
//        }

        if (num < VOICE_MAX)
        {
            GPIOB_SetBits(GPIO_Pin_6);
            DelayUs(100);
            GPIOB_ResetBits(GPIO_Pin_6);
            DelayUs(150);
            for (int i = 0; i < num; i++)
            {
                GPIOB_SetBits(GPIO_Pin_5);
                DelayUs(150);
                GPIOB_ResetBits(GPIO_Pin_5);
                DelayUs(150);
            }
        }
    }
}
/*****************************************************************************
 [函数名称]vEV3P035JPlay()
 [函数功能]EV3P035J播放音频
 [参    数]num 音频序号
 [全局变量]无
 [返 回 值]无
 [备    注]
 *****************************************************************************/
void vEV3P035JTask(void)
{
    static uint8_t times = 0;

    if (times > 0) {
        times--;
        return;
    }

    uint8_t num = 0;

    if (RB_Get_Length(&rb1) > 0) {
        RB_Read_Byte(&rb1, &num);
        num = 1 + num + 12 * baseParameters.languageType;

        if (num >= 0 && num < 38) {
            GPIOB_SetBits(GPIO_Pin_6);
            DelayUs(100);
            GPIOB_ResetBits(GPIO_Pin_6);
            DelayUs(150);

            for (int i = 0; i < num; i++) {
                GPIOB_SetBits(GPIO_Pin_5);
                DelayUs(150);
                GPIOB_ResetBits(GPIO_Pin_5);
                DelayUs(150);
            }

            PRINT("beep : %d", num);
            times = 30;
        }
    }
}

/*********************************************************************
 * @fn      TMR0_IRQHandler
 *btn_event_callback
 * @brief   TMR0中断函数
 *
 * @return  none
 */
// __INTERRUPT
// __HIGH_CODE
// void TMR2_IRQHandler(void) // TMR0 定时中断
// {
//     if (TMR2_GetITFlag(TMR0_3_IT_CYC_END)) {
//         static uint16_t times = 0;
//         uint8_t num = 0;
//         if (times == 0) {
//             // PRINT("read: %d\r\n",RB_Get_Length(&rb1));
//             if (RB_Get_Length(&rb1) > 0) {
//                 RB_Read_Byte(&rb1, &num);
//                 times = 40;
//                 num = 1 + num + 12 * para1.language;
//                 if (num < 38 && num >= 0) {

//                     GPIOB_SetBits(GPIO_Pin_6);
//                     DelayUs(200);
//                     GPIOB_ResetBits(GPIO_Pin_6);                    
//                     for (int i = 0; i < num; i++) {
//                         DelayUs(150);
//                         GPIOB_SetBits(GPIO_Pin_5);
//                         DelayUs(150);
//                         GPIOB_ResetBits(GPIO_Pin_5);                        
//                     }
//                     PRINT("beep : %d", num);                   
//                 }
//             }
//         } else {
//             times--;
//         }
//         TMR2_ClearITFlag(TMR0_3_IT_CYC_END); // 清除中断标志
//     }
// }
