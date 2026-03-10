/*
 * tm1621.c
 *
 *  Created on: 2024年4月8日
 *      Author: YYZ
 */

#include "HAL.h"

uint8_t m_au8Value[HT1628A_BYTES];

/*****************************************************************************
 [函数名称]vSendData
 [函数功能]写模式 写数据到RAM中
 [参    数]
 [全局变量]无
 [返 回 值]无
 [备    注]
 *****************************************************************************/
void TM1628A_SendData(uint8_t u8Data)
{
    uint8_t i = 0;

    TM1628A_STB_L;
    TM1628A_CLK_H;
    DelayUs(5);
    for (i = 0; i < 8; i++)
    {
        TM1628A_CLK_L;
        DelayUs(2);
        if ((u8Data & 0x01) == 1)
        {
            TM1628A_DIO_H;
        }
        else
        {
            TM1628A_DIO_L;
        }
        u8Data >>= 1;
        DelayUs(5);
        TM1628A_CLK_H;
        DelayUs(6);
    }
//    TM1628A_DIO_L;
}

/*****************************************************************************
 [函数名称]vSetSeg
 [函数功能]
 [全局变量]无
 [返 回 值]
 [备    注] u8Addr seg地址   u8Bit com地址  bOn 置1或0
 *****************************************************************************/
void vSetSeg(uint8_t u8Addr, uint8_t u8Bit, uint8_t bOn)
{
	if (u8Addr < HT1628A_BYTES)
	{
		uint8_t u8Write = 0;

		if(u8Bit>7)
		{
		    u8Bit = u8Bit-8;
		    u8Addr++;
		}

		u8Write = m_au8Value[u8Addr];

		if (bOn) // 数据写1或0
		{
			u8Write |= (0x01 << u8Bit); // 将该位置1
		}
		else
		{
			u8Write &= ~(0x01 << u8Bit); // 将该位置0
		}
//		u8Write &= 0x0F;
		m_au8Value[u8Addr] = u8Write;
	}
}
/*****************************************************************************
 [函数名称]vClearScreen
 [函数功能]
 [全局变量]无
 [返 回 值]
 [备    注]
 *****************************************************************************/
void vFreshScreen(void)
{
    TM1628A_updataDisRAM_AutoAdd(m_au8Value);
}

/*****************************************************************************
 [函数名称]vClearScreen
 [函数功能]
 [全局变量]无
 [返 回 值]
 [备    注]
 *****************************************************************************/
void vClearScreen(void)
{
	memset(&m_au8Value, 0, HT1628A_BYTES);
}

/*****************************************************************************
 [函数名称]vFillScreen
 [函数功能]
 [全局变量]无
 [返 回 值]
 [备    注]
 *****************************************************************************/
void vFillScreen(void)
{
	memset(&m_au8Value, 0xFF, HT1628A_BYTES);
}
/*****************************************************************************
 [函数名称]vSendCommand
 [函数功能]命令模式
 [参    数]
 [全局变量]无
 [返 回 值]无
 [备    注]
 *****************************************************************************/
void TM1628A_WriteCmd(uint8_t u8Command)
{
    TM1628A_STB_H;
    DelayUs(100);
    TM1628A_STB_L;

    TM1628A_SendData(u8Command);
}

void TM1628A_updataDisRAM_AutoAdd(u8 *p)
{
    uint8_t i;

    TM1628A_WriteCmd(CMD_SET_DISMODE);
    TM1628A_WriteCmd(CMD_WRITEREG_AUTOADD);
    TM1628A_WriteCmd(CMD_STARTADD);
    for(i=0;i<HT1628A_BYTES;i++)
    {
        TM1628A_SendData(p[i]);
    }
    TM1628A_WriteCmd(runState.bledDuty);
    TM1628A_STB_H;
}

void TM1628A_updataDisRAM_ManuAdd(u8 add,u8 dat)
{
    TM1628A_WriteCmd(CMD_SET_DISMODE);
    TM1628A_WriteCmd(CMD_WRITEREG_MANUADD);
    TM1628A_WriteCmd(CMD_STARTADD | add);
    TM1628A_SendData(dat);
    TM1628A_WriteCmd(runState.bledDuty);
    TM1628A_STB_H;
}

void TM1628A_updataDisRAM_ManuAddAll(u8 add,u8 *dat)
{
    uint8_t i;

    TM1628A_WriteCmd(CMD_SET_DISMODE);
    TM1628A_WriteCmd(CMD_WRITEREG_MANUADD);
    for(i=0;i<HT1628A_BYTES;i++)
    {
        TM1628A_WriteCmd(CMD_STARTADD | (add+i));
        TM1628A_SendData(dat[i]);
    }
    TM1628A_WriteCmd(runState.bledDuty);
    TM1628A_STB_H;
}

void TM1628A_GpioInit(void)
{
    GPIOB_SetBits(GPIO_Pin_0);
    GPIOB_SetBits(GPIO_Pin_1);
    GPIOB_SetBits(GPIO_Pin_3);
    GPIOB_ModeCfg(GPIO_Pin_0 , GPIO_ModeOut_PP_5mA);
    GPIOB_ModeCfg(GPIO_Pin_1 , GPIO_ModeOut_PP_5mA);
    GPIOB_ModeCfg(GPIO_Pin_3 , GPIO_ModeOut_PP_5mA);
    DelayMs(2);
}
/*****************************************************************************
 [函数名称]vTM1621Init
 [函数功能]命令模式
 [参    数]
 [全局变量]无
 [返 回 值]无
 [备    注]
 *****************************************************************************/
void TM1628A_Init(void)
{
    memset(m_au8Value,0,sizeof(m_au8Value));
    Lcd_Bk_On();
//    TM1628A_WriteCmd(CMD_SET_DISMODE);      //显示模式  7位10段
//    TM1628A_WriteCmd(CMD_WRITEREG_AUTOADD); //数据命令   设置数据读写
//    TM1628A_WriteCmd(CMD_STARTADD);         //地址命令设置
//    TM1628A_WriteCmd(CMD_DISCTRL);          //显示控制命令  对比度和开显示
    TM1628A_updataDisRAM_AutoAdd(m_au8Value);
//    TM1628A_updataDisRAM_ManuAdd(0x0,0);
//    TM1628A_updataDisRAM_ManuAdd(0x1,0);
//    TM1628A_updataDisRAM_ManuAdd(0x2,0);
//    TM1628A_updataDisRAM_ManuAdd(0x3,0);
//    TM1628A_updataDisRAM_ManuAdd(0x4,0);
//    TM1628A_updataDisRAM_ManuAdd(0x5,0);
//    TM1628A_updataDisRAM_ManuAdd(0x6,0);
//    TM1628A_updataDisRAM_ManuAdd(0x7,0);
//    TM1628A_updataDisRAM_ManuAdd(0x8,0);
//    TM1628A_updataDisRAM_ManuAdd(0x9,0);
//    TM1628A_updataDisRAM_ManuAdd(0xA,0);
//    TM1628A_updataDisRAM_ManuAdd(0xB,0);
//    TM1628A_updataDisRAM_ManuAdd(0xC,0);
//    TM1628A_updataDisRAM_ManuAdd(0xD,0);
//    DelayUs(20);
}
