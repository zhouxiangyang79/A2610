/*
 * tm1621.h
 *
 *  Created on: 2023年6月16日
 *      Author: wx030
 */

#ifndef INCLUDE_TM1621_H_
#define INCLUDE_TM1621_H_

#include "config.h"

// A2610 TM1628 引脚配置: PA14(DATA), PA3(STB), PA15(WR/CLK)
#define TM1628A_DIO_L   GPIOA_SetBits(GPIO_Pin_14)
#define TM1628A_DIO_H   GPIOA_ResetBits(GPIO_Pin_14)

#define TM1628A_CLK_L   GPIOA_SetBits(GPIO_Pin_15)
#define TM1628A_CLK_H   GPIOA_ResetBits(GPIO_Pin_15)

#define TM1628A_STB_L   GPIOA_SetBits(GPIO_Pin_3)
#define TM1628A_STB_H   GPIOA_ResetBits(GPIO_Pin_3)

#define CMD_SET_DISMODE                             0x03
#define CMD_WRITEREG_AUTOADD                        0x40    //地址自动增加  普通模式
//#define CMD_READKEY_MODE                            0x42
//#define CMD_READSW_MODE                             0x43
#define CMD_WRITEREG_MANUADD                        0x44
#define CMD_STARTADD                                0xC0
#define CMD_DISCTRL                                 0x8F    //0x8C 脉冲宽度  14/16 显示开

#define SEG_NUM                                     9       //数码管段
#define GRID_NUM                                    7       //位
#define HT1628A_BYTES                               2*GRID_NUM

void vClearScreen(void);
void vFreshScreen(void);
void vSetSeg(uint8_t u8Addr, uint8_t u8Bit, uint8_t bOn);
void TM1628A_GpioInit(void);
void TM1628A_Init(void);
void TM1628A_updataDisRAM_AutoAdd(uint8_t *p);
void TM1628A_updataDisRAM_ManuAdd(uint8_t add,u8 dat);
void TM1628A_updataDisRAM_ManuAddAll(u8 add,u8 *dat);
void TM1628A_ReadKeyValue(uint8_t *p);
uint8_t GetTM1628A_KeyBuf(uint8_t pt,uint8_t set_val);

#endif /* INCLUDE_TM1621_H_ */
