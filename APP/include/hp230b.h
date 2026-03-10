/*
 * hp230b.h
 *
 *  Created on: 2024年11月20日
 *      Author: yyz
 */
#ifndef INCLUDE_HP203B_H_
#define INCLUDE_HP203B_H_

#include "config.h"

#define SET_SCL_LOW()       GPIOA_ResetBits(GPIO_Pin_2)
#define SET_SCL_HIGH()      GPIOA_SetBits(GPIO_Pin_2)

#define SET_SDA_LOW()       GPIOA_ResetBits(GPIO_Pin_1)
#define SET_SDA_HIGH()      GPIOA_SetBits(GPIO_Pin_1)
#define PIN_SDA_IN()        GPIOA_ModeCfg(GPIO_Pin_1, GPIO_ModeIN_Floating)
#define PIN_SDA_OUT()       GPIOA_ModeCfg(GPIO_Pin_1, GPIO_ModeOut_PP_5mA)
#define READ_PIN_SDA()      GPIOA_ReadPortPin(GPIO_Pin_1)

#define HP20X_ADDR 0xEC //CSB_PIN = 1

//命令
#define CMD_SOFT_RST 0x06   //复位

//选择通道
#define CMD_CH_PT               0 << 0  //压力+温度通道
//#define CMD_CH_HT               1 << 0  //高度+温度通道
#define CMD_CH_T                2 << 0  //温度通道

//转换时间
#define CMD_OSR_128         5 << 2
#define CMD_OSR_256         4 << 2
#define CMD_OSR_512         3 << 2
#define CMD_OSR_1024        2 << 2
#define CMD_OSR_2048        1 << 2
#define CMD_OSR_4096        0 << 2

#define Delay_OSR_128  5
#define Delay_OSR_256  10
#define Delay_OSR_512  17
#define Delay_OSR_1024 35
#define Delay_OSR_2048 70
#define Delay_OSR_4096 140

#define CMD_READ_PT  0x10   //读取温度和压力
#define CMD_READ_AT  0x11 //读取温度和海拔值
#define CMD_READ_P   0x30   //读取气压值
#define CMD_READ_A   0x31 //读取高度值
#define CMD_READ_T   0x32   //读取温度值
#define CMD_ANA_CAL  0x28 //内部校准

#define REG_INT_EN      0x0B
#define REG_INT_EN_PAEN 5
#define REG_INT_EN_TEN 4
#define REG_INT_EN_PATRAVEN 3
#define REG_INT_EN_TTRAVEN 2
#define REG_INT_EN_PAWINEN 1
#define REG_INT_EN_TWINEN 0
#define REG_INT_CFG     0x0C
#define REG_INT_SRC     0x0D
#define REG_INT_SRC_THERR 7
#define REG_INT_SRC_DEVRDY 6
#define REG_INT_SRC_PARDY 5
#define REG_INT_SRC_TRDY 4
#define REG_INT_SRC_PATRAV 3
#define REG_INT_SRC_TTRAV2
#define REG_INT_SRC_PAWIN 1
#define REG_INT_SRC_TWIN 0
#define REG_INT_DIR     0x0E
#define REG_PARA            0x0F

#define HP20X_SET_OSR 0x40  //设定OSR

#define HP20X_WR_REG_MODE     0xC0  //写控制器
#define HP20X_RD_REG_MODE     0x80  //读控制器

void HP230BIO_Init(void);
void HP203X_WriteCmd(uint8_t cmd);
void HP203X_WriteReg(uint8_t reg,uint8_t dat);
uint8_t HP203X_ReadReg(uint8_t reg);
uint32_t HP203X_Read3Bytes(uint8_t cmd);
uint8_t HP203X_RST(void);
uint32_t HP203X_ReadTemperature(void);
uint32_t HP203X_ReadPress(void);
uint32_t HP203X_ReadAltimet(void);
void HP203X_ReadAltimetAndTemperature(uint32_t *pressure, uint32_t *temperature);
void get_HP203BAH(void);
uint8_t HP203X_init(void);

#endif /* INCLUDE_TM1621_H_ */
