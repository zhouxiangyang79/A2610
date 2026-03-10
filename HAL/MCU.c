/********************************** (C) COPYRIGHT *******************************
 * File Name          : MCU.c
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2022/01/18
 * Description        : 硬件任务处理函数及BLE和硬件初始化
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/******************************************************************************/
/* 头文件包含 */
#include "HAL.h"
#include "date_time.h"
#include "hp230b.h"

tmosTaskID halTaskID;
//extern enum ec11_keyvalue ec11_key;
//unsigned char key[16]={0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x41,0x42,0x43,0x44,0x45,0x46};
/*******************************************************************************
 * @fn      Lib_Calibration_LSI
 *
 * @brief   内部32k校准
 *
 * @param   None.
 *
 * @return  None.
 */
void Lib_Calibration_LSI(void)
{
    Calibration_LSI(Level_64);
}

#if (defined(BLE_SNV)) && (BLE_SNV == TRUE)
/*******************************************************************************
 * @fn      Lib_Read_Flash
 *
 * @brief   Callback function used for BLE lib.
 *
 * @param   addr - Read start address
 * @param   num - Number of units to read (unit: 4 bytes)
 * @param   pBuf - Buffer to store read data
 *
 * @return  None.
 */
uint32_t
Lib_Read_Flash(uint32_t addr, uint32_t num, uint32_t* pBuf)
{
    EEPROM_READ(addr, pBuf, num * 4);
    return 0;
}

/*******************************************************************************
 * @fn      Lib_Write_Flash
 *
 * @brief   Callback function used for BLE lib.
 *
 * @param   addr - Write start address
 * @param   num - Number of units to write (unit: 4 bytes)
 * @param   pBuf - Buffer with data to be written
 *
 * @return  None.
 */
uint32_t
Lib_Write_Flash(uint32_t addr, uint32_t num, uint32_t* pBuf)
{
    EEPROM_ERASE(addr, num * 4);
    EEPROM_WRITE(addr, pBuf, num * 4);
    return 0;
}
#endif

/*******************************************************************************
 * @fn      CH57X_BLEInit
 *
 * @brief   BLE 库初始化
 *
 * @param   None.
 *
 * @return  None.
 */
void CH58X_BLEInit(void)
{
    uint8_t i;
    bleConfig_t cfg;
    if (tmos_memcmp(VER_LIB, VER_FILE, strlen(VER_FILE)) == FALSE) {
        PRINT("head file error...\n");
        while (1)
            ;
    }
    SysTick_Config(SysTick_LOAD_RELOAD_Msk);
    PFIC_DisableIRQ(SysTick_IRQn);

    tmos_memset(&cfg, 0, sizeof(bleConfig_t));
    cfg.MEMAddr = (uint32_t)MEM_BUF;
    cfg.MEMLen = (uint32_t)BLE_MEMHEAP_SIZE;
    cfg.BufMaxLen = (uint32_t)BLE_BUFF_MAX_LEN;
    cfg.BufNumber = (uint32_t)BLE_BUFF_NUM;
    cfg.TxNumEvent = (uint32_t)BLE_TX_NUM_EVENT;
    cfg.TxPower = (uint32_t)BLE_TX_POWER;
#if (defined(BLE_SNV)) && (BLE_SNV == TRUE)
    cfg.SNVAddr = (uint32_t)BLE_SNV_ADDR;
    cfg.readFlashCB = Lib_Read_Flash;
    cfg.writeFlashCB = Lib_Write_Flash;
#endif
#if (CLK_OSC32K)
    cfg.SelRTCClock = (uint32_t)CLK_OSC32K;
#endif
    cfg.ConnectNumber = (PERIPHERAL_MAX_CONNECTION & 3) | (CENTRAL_MAX_CONNECTION << 2);
    cfg.srandCB = SYS_GetSysTickCnt;
#if (defined TEM_SAMPLE) && (TEM_SAMPLE == TRUE)
    cfg.tsCB = HAL_GetInterTempValue; // 根据温度变化校准RF和内部RC( 大于7摄氏度 )
#if (CLK_OSC32K)
    cfg.rcCB = Lib_Calibration_LSI; // 内部32K时钟校准
#endif
#endif
#if (defined(HAL_SLEEP)) && (HAL_SLEEP == TRUE)
    cfg.WakeUpTime = WAKE_UP_RTC_MAX_TIME;
    cfg.sleepCB = CH58X_LowPower; // 启用睡眠
#endif
#if (defined(BLE_MAC)) && (BLE_MAC == TRUE)
    for (i = 0; i < 6; i++) {
        cfg.MacAddr[i] = MacAddr[5 - i];
    }
#else
    {
        uint8_t MacAddr[6];
        GetMACAddress(MacAddr);
        for (i = 0; i < 6; i++) {
            cfg.MacAddr[i] = MacAddr[i]; // 使用芯片mac地址
        }
    }
#endif
    if (!cfg.MEMAddr || cfg.MEMLen < 4 * 1024) {
        while (1)
            ;
    }
    i = BLE_LibInit(&cfg);
    if (i) {
        PRINT("LIB init error code: %x ...\n", i);
        while (1)
            ;
    }
}

/*******************************************************************************
 * @fn      HAL_ProcessEvent
 *
 * @brief   硬件层事务处理
 *
 * @param   task_id - The TMOS assigned task ID.
 * @param   events  - events to process.  This is a bit map and can
 *                      contain more than one event.
 *
 * @return  events.
 */
#define TIMESET

tmosEvents
HAL_ProcessEvent(tmosTaskID task_id, tmosEvents events)
{
    uint8_t* msgPtr;
    uint32_t ykq, time;
    static uint32_t ykq_last, time_last = 0;
    if (events & SYS_EVENT_MSG) { // 处理HAL层消息，调用tmos_msg_receive读取消息，处理完成后删除消息。
        msgPtr = tmos_msg_receive(task_id);
        if (msgPtr) {
            /* De-allocate */
            tmos_msg_deallocate(msgPtr);
        }
        return events ^ SYS_EVENT_MSG;
    }

    //无按键操作时，状态切换和控制
    if(events & HAL_RUN_EVENT)
    {
        main_running_task();
        tmos_start_task(halTaskID, HAL_RUN_EVENT, MS1_TO_SYSTEM_TIME(200));
        return events ^ HAL_RUN_EVENT;
    }

    if (events & HAL_NRF_EVENT) {
        if (rfGetOk == 1) {
            rfGetOk = 0;

            if(GetDisSet433OnOff()==1)
            {
                ykq = rev_buf[0]*256*256*256 + rev_buf[1]*256*256 + rev_buf[2]*256 + rev_buf[3];
                if(ykq!=CO_OFF_CODE)
                {
                    if((rev_buf[2]&0x05) == 0x05) rev_buf[2] = rev_buf[2] & 0xF4;
                    rev_buf[2] = rev_buf[2]&0x0F;
                    ykq = rev_buf[0] * 256 * 256 + rev_buf[1] * 256 + rev_buf[2];
                }
                time = (RTC_GetCycle32k() / 2000);//R16_RTC_CNT_2S * 2 + ((R16_RTC_CNT_32K < 0x8000) ? 0 : 1);
                if (ykq_last != ykq) {
                    ykq_key = ykq;
                    time_last = time;
                } else if (time_last < time && (time - time_last) > 3) {
                    ykq_key = ykq;
                    time_last = time;
                } else if (time_last > time && ((time + (24 * 60 * 60)) - time_last) > 3) {
                    ykq_key = ykq;
                    time_last = time;
                } else {
                    // ykq_key = 0xFFFFFFFF;
                }
                ykq_last = ykq;
            }else
            {
                ykq = rev_buf[0]*256*256*256 + rev_buf[1]*256*256 + rev_buf[2]*256 + rev_buf[3];
                if(runState.disIndex == DISINDEX_ENGMODE_REMOTEMATCH || ykq==CO_OFF_CODE)
                {
                    if(ykq!=CO_OFF_CODE)
                    {
                        if((rev_buf[2]&0x05) == 0x05) rev_buf[2] = rev_buf[2] & 0xF4;
                        rev_buf[2] = rev_buf[2]&0x0F;
                        ykq = rev_buf[0] * 256 * 256 + rev_buf[1] * 256 + rev_buf[2];
                    }
                    time = (RTC_GetCycle32k() / 2000);//R16_RTC_CNT_2S * 2 + ((R16_RTC_CNT_32K < 0x8000) ? 0 : 1);
                    if (ykq_last != ykq) {
                        ykq_key = ykq;
                        time_last = time;
                    } else if (time_last < time && (time - time_last) > 3) {
                        ykq_key = ykq;
                        time_last = time;
                    } else if (time_last > time && ((time + (24 * 60 * 60)) - time_last) > 3) {
                        ykq_key = ykq;
                        time_last = time;
                    } else {
                        // ykq_key = 0xFFFFFFFF;
                    }
                    ykq_last = ykq;
                }
            }
        }
#if WWDG_EN==1
        WWDG_SetCounter(0);//喂狗
#endif
        tmos_start_task(halTaskID, HAL_NRF_EVENT, MS1_TO_SYSTEM_TIME(50));
        return events ^ HAL_NRF_EVENT;
    }
    //按键
    if (events & HAL_BUTTON_EVENT) {
        button_polling();
        tmos_start_task(halTaskID, HAL_BUTTON_EVENT, MS1_TO_SYSTEM_TIME(100));
        return events ^ HAL_BUTTON_EVENT;
    }
    //温度获取
    if (events & HAL_TEMP_EVENT) {
        get_CabTemp();
        get_HP203BAH();

        tmos_start_task(halTaskID, HAL_TEMP_EVENT, MS1_TO_SYSTEM_TIME(200));
        return events ^ HAL_TEMP_EVENT;
    }

    if (events & HAL_REG_INIT_EVENT) {
#if (defined BLE_CALIBRATION_ENABLE) && (BLE_CALIBRATION_ENABLE == TRUE) // 校准任务，单次校准耗时小于10ms
        BLE_RegInit(); // 校准RF
#if (CLK_OSC32K)
        Lib_Calibration_LSI(); // 校准内部RC
#endif
        tmos_start_task(halTaskID, HAL_REG_INIT_EVENT,
            MS1_TO_SYSTEM_TIME(BLE_CALIBRATION_PERIOD));
#endif
        return events ^ HAL_REG_INIT_EVENT;
    }
    return 0;
}

/*******************************************************************************
 * @fn      HAL_Init
 *
 * @brief   硬件初始化
 *
 * @param   None.
 *
 * @return  None.
 */
void HAL_Init()
{
    halTaskID = TMOS_ProcessEventRegister(HAL_ProcessEvent);
    HAL_TimeInit();
    // PFIC_EnableAllIRQ();
#if (defined HAL_SLEEP) && (HAL_SLEEP == TRUE)
    HAL_SleepInit();
#endif

#if (defined HAL_KEY) && (HAL_KEY == TRUE)
    user_button_init();
    AdcInit();
    vPARA_INIT();   //系统参数初始化
    runState_Init();
    vHAL_CMT2220INIT();
    UART1_INIT();
    vEV3P035JInit();
    Hal_Lcd_Init();
    HP230BIO_Init();
#endif

#if (defined BLE_CALIBRATION_ENABLE) && (BLE_CALIBRATION_ENABLE == TRUE)
    tmos_start_task(halTaskID, HAL_REG_INIT_EVENT,
        MS1_TO_SYSTEM_TIME(BLE_CALIBRATION_PERIOD)); // 添加校准任务，单次校准耗时小于10ms
#endif
    tmos_start_task(halTaskID, HAL_BUTTON_EVENT, MS1_TO_SYSTEM_TIME(100));
    tmos_start_task(halTaskID, HAL_RUN_EVENT, MS1_TO_SYSTEM_TIME(200));
    tmos_start_task(halTaskID, HAL_TEMP_EVENT, MS1_TO_SYSTEM_TIME(200));
    tmos_start_task(halTaskID, HAL_NRF_EVENT, MS1_TO_SYSTEM_TIME(10));
}

/*******************************************************************************
 * @fn      HAL_GetInterTempValue
 *
 * @brief   获取内部温感采样值，如果使用了ADC中断采样，需在此函数中暂时屏蔽中断.
 *
 * @return  内部温感采样值.
 */
uint16_t
HAL_GetInterTempValue(void)
{
    uint8_t sensor, channel, config, tkey_cfg;
    uint16_t adc_data;

    tkey_cfg = R8_TKEY_CFG;
    sensor = R8_TEM_SENSOR;
    channel = R8_ADC_CHANNEL;
    config = R8_ADC_CFG;
    ADC_InterTSSampInit();
    R8_ADC_CONVERT |= RB_ADC_START;
    while (R8_ADC_CONVERT & RB_ADC_START);
    adc_data = R16_ADC_DATA;
    R8_TEM_SENSOR = sensor;
    R8_ADC_CHANNEL = channel;
    R8_ADC_CFG = config;
    R8_TKEY_CFG = tkey_cfg;

    return (adc_data);
}

/******************************** endfile @ mcu ******************************/
