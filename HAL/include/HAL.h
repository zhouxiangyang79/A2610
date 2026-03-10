/********************************** (C) COPYRIGHT *******************************
 * File Name          : HAL.h
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2016/05/05
 * Description        :
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/******************************************************************************/
#ifndef __HAL_H
#define __HAL_H

#ifdef __cplusplus
extern "C"
{
#endif

//#include "LED.h"
#include "RTC.h"
#include "SLEEP.h"
#include "config.h"
#include <time.h>
#include "device_type.h"
#include "cmt2220ls.h"
#include "date_time.h"
#include "lcd.h"
#include "multi_button.h"
#include "multi_timer.h"
#include "parameter.h"
#include "ring_buffer.h"
#include "temp.h"
#include "tm1628a.h"
#include "uart1.h"
#include <math.h>
#include "button.h"
#include "peripheral.h"
#include "running.h"
#include "EV3P035J.h"

/* hal task Event */
#define HAL_RUN_EVENT 0x0001
#define HAL_DATA_EVENT 0x0002
#define HAL_NRF_EVENT 0x0004
#define HAL_EV3_EVENT 0x0008
#define HAL_BUTTON_EVENT 0x0010
#define HAL_REG_INIT_EVENT 0x2000
#define HAL_TEMP_EVENT 0x4000

#define THREE_LINES
/*********************************************************************
 * GLOBAL VARIABLES
 */
//extern tmosTaskID halTaskID;
//extern PARA1 para1;
//extern PARA1 set_para1;
//extern PARA2 para2;
//extern uint32_t systicks;
//extern uint8_t kj_flage;
//extern enum ec11_keyvalue ec11_key;
//extern uint8_t disp_flage;
extern uint32_t rf_code[4];
extern uint8_t rfGetOk;//uint16_t getdata;
extern uint8_t rev_buf[10];
//extern uint8_t setlevel;
//extern int16_t settemp;
extern uint32_t ykq_key;
extern uint32_t ykq_key_last;
//
//extern uint8_t ble_key;
/*********************************************************************
 * GLOBAL FUNCTIONS
 */

/**
 * @brief   硬件初始化
 */
extern void
HAL_Init(void);
extern void main_running_task(void);
/**
 * @brief   硬件层事务处理
 *
 * @param   task_id - The TMOS assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 */
extern tmosEvents
HAL_ProcessEvent(tmosTaskID task_id, tmosEvents events);

/**
 * @brief   BLE 库初始化
 */
extern void
CH58X_BLEInit(void);

/**
 * @brief   获取内部温感采样值，如果使用了ADC中断采样，需在此函数中暂时屏蔽中断.
 *
 * @return  内部温感采样值.
 */
extern uint16_t
HAL_GetInterTempValue(void);

/**
 * @brief   内部32k校准
 */
extern void
Lib_Calibration_LSI(void);

/*********************************************************************
 *********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
