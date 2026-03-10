/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.1
 * Date               : 2020/08/06
 * Description        : 外设从机应用主函数及任务系统初始化
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/******************************************************************************/
/* 头文件包含 */
#include "CONFIG.h"
#include "HAL.h"
#include "gattprofile.h"
#include "peripheral.h"
#include "RF_PHY.h"
#include "wifi_a2610.h"

/*********************************************************************
 * GLOBAL TYPEDEFS
 */
__attribute__((aligned(4)))          uint32_t MEM_BUF[BLE_MEMHEAP_SIZE / 4];

#if(defined(BLE_MAC)) && (BLE_MAC == TRUE)
const uint8_t MacAddr[6] =
{	0x84, 0xC2, 0xE4, 0x03, 0x02, 0x02};
#endif

/* 用于APP判断文件有效性 */
const uint32_t Address = 0xFFFFFFFF;

__attribute__((aligned(4))) uint32_t Image_Flag __attribute__((section(".ImageFlag"))) = (uint32_t)&Address;

uint32_t systicks = 0;
/*********************************************************************
 * @fn      Main_Circulation
 *
 * @brief   主循环
 *
 * @return  none
 */
__HIGH_CODE
__attribute__((noinline))
void Main_Circulation()
{
	while(1)
	{
		TMOS_SystemProcess();
	}
}

/*********************************************************************
 * @fn      main
 *
 * @brief   主函数
 *
 * @return  none
 */
int main(void)
{
#if(defined(DCDC_ENABLE)) && (DCDC_ENABLE == TRUE)
	PWR_DCDCCfg(ENABLE);
#else
	PWR_DCDCCfg(DISABLE);
#endif
	sys_safe_access_enable();
    R32_MISC_CTRL = (R32_MISC_CTRL&~7) | 5 | (3<<25);
    sys_safe_access_disable();
	SetSysClock(CLK_SOURCE_PLL_60MHz);
	HSECFG_Capacitance(HSECap_20p);
	vEV3P035JInit();
#if(defined(HAL_SLEEP)) && (HAL_SLEEP == TRUE)
	GPIOA_ModeCfg(GPIO_Pin_All, GPIO_ModeIN_PU);
	GPIOB_ModeCfg(GPIO_Pin_All, GPIO_ModeIN_PU);
#endif
	//SysTick_Config (60000);
#ifdef DEBUG
	//GPIOPinRemap (ENABLE, RB_PIN_UART0);
	GPIOB_SetBits(bTXD0);
	GPIOB_ModeCfg(bTXD0, GPIO_ModeOut_PP_5mA);
	UART0_DefInit();
#endif
	PRINT("%s\n", VER_LIB);
	PRINT("A2610 CH584 WiFi Version\n");
	CH58X_BLEInit();
	HAL_Init();
	GAPRole_PeripheralInit();
	Peripheral_Init();
	
	// A2610 WiFi 初始化
	WiFi_Init();
	PRINT("WiFi Init OK\n");
	
#if WWDG_EN==1
    WWDG_SetCounter(0);//喂狗
    WWDG_ClearFlag();//清除中断标志
    WWDG_ResetCfg(ENABLE);//使能看门狗复位
#endif
	Main_Circulation();
}

// void m_timer_init()
// {
//   TMR2_TimerInit (FREQ_SYS / 20000);
//   TMR2_ITCfg(ENABLE, TMR0_3_IT_CYC_END);
//   PFIC_EnableIRQ (TMR2_IRQn);
// }

// /*********************************************************************
//  * @fn      TMR0_IRQHandler
//  *btn_event_callback
//  * @brief   TMR0中断函数
//  *
//  * @return  none
//  */
// __INTERRUPT
// __HIGH_CODE
// void TMR2_IRQHandler(void) // TMR0 定时中断
// {
//     if (TMR2_GetITFlag(TMR0_3_IT_CYC_END)) {
//         timer_ticks();
//         TMR2_ClearITFlag(TMR0_3_IT_CYC_END); // 清除中断标志
//     }
// }

//__INTERRUPT
//__HIGH_CODE
//void
//SysTick_Handler ()
//{
//  systicks++;
//  timer_ticks ();
//  SysTick->SR = 0; //清除中断标志
//}
uint32_t __get_MEPC(void);
uint32_t __get_MTVAL(void);
uint32_t __get_MCAUSE(void);

__INTERRUPT
__HIGH_CODE
__attribute__((weak))

void HardFault_Handler(void)
{
	GPIOA_ModeCfg(1 << 9, GPIO_ModeOut_PP_5mA);
	UART1_DefInit();
	UART1_BaudRateCfg(115200);     //串口初始化，示例是串口1，波特兰115200

//	uint32_t mepc = __get_MEPC();
//	uint32_t mtval = __get_MTVAL();
//	uint32_t mcause = __get_MCAUSE();
//	printf("mepc   = %08x\nmtval  = %08x\nmcause = %08x\n", mepc, mtval,
//			mcause);  //异常产生硬件错误中断，打印PC指针
//
//	PRINT("%s \n", __FUNCTION__);
//	FLASH_ROM_SW_RESET();
	sys_safe_access_enable();
	R16_INT32K_TUNE = 0xFFFF;
	sys_safe_access_enable();
	R8_RST_WDOG_CTRL |= RB_SOFTWARE_RESET; //软复位
	sys_safe_access_disable();
	while(1);

}
/******************************** endfile @ main ******************************/
