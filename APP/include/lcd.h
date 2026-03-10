/*
 * lcd.h
 *
 *  Created on: 2024年2月26日
 *      Author: yyz
 */

#ifndef INCLUDE_LCD_H_
#define INCLUDE_LCD_H_
#include "device_type.h"
#include "date_time.h"

typedef enum
{
    A1=0,B1,C1,D1,E1,F1,G1,IN1,LG1,
    A2,B2,C2,D2,E2,F2,G2,IN2,IN3,
    A3,B3,C3,D3,E3,F3,G3,FAN1,LG2,
    A4,B4,C4,D4,E4,F4,G4,LG3,LG4,
    LV5,LV6,LV1,LV2,LV3,LV4,LV7,LV8,LV9,
    LV10,WR,RF,TC,AU,LV,DIO1,CK,HT,
    DIO2,FAN2,TF,M,OUT1,FAN3,OUT2,OUT3,NUL,
} tm1621_id;

typedef enum
{
	A = 10, B, C, D, E, F, H, L, N, U, P, O, R, S, T, r, n, u, cc, LINE, NODISP,POINT,G,
} LED_CODE;

typedef enum
{
    DISINDEX_NULL=0,
    DISINDEX_PWROFFED,              //关机
    DISINDEX_SELFCHECK,             //上电自检显示
    DISINDEX_VERSION,               //版本号显示
    DISINDEX_REMOTECLEAR,           //遥控器准备清除
//    DISINDEX_REMOTECLEAR_SUC,       //遥控器清除成功
    DISINDEX_AUTOTMP_REMOTETEMP,    //自动恒温时是否使能遥控温度传入主机
    DISINDEX_PWRON_ANIMATION,       //开机动画显示
    DISINDEX_PWROFF_ANIMATION,      //关机动画显示
    DISINDEX_RUNNING,               //运行时显示
#if CO_VALUE==1
    DISINDEX_CO_VALUE,              //CO数值显示
#endif
    DISINDEX_CO_OVERWARNING,        //CO超标报警
    DISINDEX_RUNNING_FAULT,         //运行时故障显示
    DISINDEX_OFFING,                //正在关机中间显示off闪烁
    DISINDEX_FAULTTYPE,             //故障显示
    DISINDEX_WAITON,                //室内温度闪烁显示

    //工程模式
    DISINDEX_ENGMODE_HEATERFW,      //主板版本号
    DISINDEX_ENGMODE_FAULTTYPE,     //运行时故障码
    DISINDEX_ENGMODE_SHELLTEMPER,   //壳体温度
    DISINDEX_ENGMODE_SUPVOL,        //电源电压
    DISINDEX_ENGMODE_RUNLEVEL,      //加热器运行档位
    DISINDEX_ENGMODE_ROOMTEMP,      //驾驶室温度
    DISINDEX_ENGMODE_ALTITUDE,      //海拔高度
    DISINDEX_ENGMODE_MANUPUMP,      //手动泵油
    DISINDEX_ENGMODE_REMOTEMATCH,   //遥控匹配
    DISINDEX_ENGMODE_BLEMATCH,      //蓝牙匹配
    DISINDEX_ENGMODE_REST,          //参数初始化
#if VCC_CHICK==1
    DISINDEX_ENGMODE_VCC,           //电源纹波ad显示
#endif
    DISINDEX_ENGMODE_MAX,

    DISINDEX_MANUPUMP_STATE,        //手动泵油  300s倒计时
    DISINDEX_REMOTEMATCH_STATE,     //遥控匹配
    DISINDEX_BLEMATCH_STATE,        //蓝牙匹配
    DISINDEX_VENTILATE_STATE,       //通风

    DISINDEX_CFGCTRLPARAM_VOICE_SW, //语音使能  语种选择

    //基础设置
    DISINDEX_CFGCTRLPARAM_RTC,      //当前时间设置
    DISINDEX_CFGCTRLPARAM_TIMER,    //定时开机时间
    DISINDEX_CFGCTRLPARAM_TIMER_RUNTIME,//定时开机运行时间
    DISINDEX_CFGCTRLPARAM_TIMER_SW, //定时开机使能
    DISINDEX_CFGCTRLPARAM_TEMPEROFFSET, //温度偏差设置
//    DISINDEX_CFGCTRLPARAM_OILCASL_VOLUME,//油箱容积选择
//    DISINDEX_CFGCTRLPARAM_OILPUMP_TYPE, //油泵类型选择
    DISINDEX_CFGCTRLPARAM_BLE_ONOFF,    //蓝牙开关
    DISINDEX_CFGCTRLPARAM_433_ONOFF,    //433遥控开关
    DISINDEX_CFGCTRLPARAM_TEMPER_STANDBY_SW,//温度自动控制使能
    DISINDEX_CFGCTRLPARAM_C_TO_F,           //温度单位切换
    DISINDEX_CFGCTRLPARAM_BLED_CTL,     //背光时间设置
#if CO_VALUE==1
    DISINDEX_CFGCTRLPARAM_CO_WARNING,   //CO报警阈值
#endif
//    DISINDEX_CFGCTRLPARAM_OILCASL_FULL, //手动将油箱置满，用于计算剩余油量
    DISINDEX_CFG_433CO_OFF,             //433检测CO固定关机码
    DISINDEX_CFGCTRLPARAM_MAX,

    DISINDEX_CFGCTRLPARAM_BLE_SW,       //蓝牙使能
    DISINDEX_CFGCTRLPARAM_REMOTE_SW,    //遥控使能
    DISINDEX_BLEMODULE_NAME,            //蓝牙名称
    DISINDEX_SECRETERROR,               //密码错误

    DISINDEX_REALTIME_STANDBY,          //
    DISINDEX_VOICE_SW_QUICK,            //
} DIS_INDEX;
#define K_VALUE(m, n) (m * 16 + n)
void Hal_Lcd_Init(void);
void Lcd_Bk_On(void);
void Lcd_Bk_Off(void);
void Lcd_Set_Segbyid(tm1621_id id, uint8_t bOn);
void Lcd_Show_Num(uint8_t u8data, uint8_t addr);
void Lcd_Show_Hight(uint16_t height);
void Lcd_Show_RemoteMatctState(uint8_t state);
void Lcd_Show_Temp(int16_t temp, uint8_t bON, uint8_t cf);
void Lcd_Show_SetTemp(uint8_t blink, uint8_t cf);
void Lcd_Show_Time(struct tm time,uint8_t timerstate);
void Lcd_Show_LanguageState(uint8_t state);
void Lcd_Show_Level(uint8_t level,uint8_t s48);
uint8_t Lcd_Show_PwrOnAnimation(void);
uint8_t Lcd_Show_PwrOffAnimation(void);
uint8_t Lcd_Show_PwrOffing(void);
void Lcd_Show_Airin(uint8_t bON);
//void vLCD_SHOW_AIROUT(uint8_t bON);
void Lcd_Show_Fan(uint8_t bON);
void Lcd_Show_Oil(uint8_t n);
void Lcd_Show_Settime(struct tm *sttime, uint8 bON);
void Lcd_Show_Setruntime(uint16_t sttime, uint8 bON);

void Lcd_ShowRunning(void);
void Lcd_ShowRunning_Fault(void);
void Lcd_ShowCO_Value(void);
void Lcd_ShowCO_OverWarning(void);
void Lcd_ShowEngMode_HeaterFW(void);
void Lcd_ShowEngMode_FaultType(void);
void Lcd_ShowEngMode_ShellTemper(void);
void Lcd_ShowEngMode_SupVol(void);
void Lcd_ShowEngMode_RunLevel(void);
void Lcd_ShowEngMode_RoomTemp(void);
void Lcd_ShowEngMode_Altitude(void);
void Lcd_ShowEngMode_ManUpUmp(void);
void Lcd_ShowEngMode_RemoteMatch(void);
void Lcd_ShowEngMode_BleMatch(void);
void Lcd_ShowEngMode_Rest(void);
void Lcd_ShowEngMode_Vcc(void);
void Lcd_Show_Round(uint8_t bon);
void Lcd_Clear_Round(void);
void Lcd_Show_Step(void);
void LcdShowCfgCtrParam_RTC(void);
void LcdShowCfgCtrParam_Timer(void);
void LcdShowCfgCtrParam_Timer_RunTime(void);
void LcdShowCfgCtrParam_Timer_Sw(void);
void LcdShowCfgCtrParam_Voice_Sw(void);
void LcdShowCfgCtrParam_TemperOffSet(void);
//void LcdShowCfgCtrParam_Oilcasl_Volume(void);
//void LcdShowCfgCtrParam_Oilpump_Type(void);
void LcdShowCfgCtrParam_BLEOnOff(void);
void LcdShowCfgCtrParam_433OnOff(void);
void LcdShowCfgCtrParam_Temper_StandbySW(void);
void LcdShowCfgCtrParam_CtoF(void);
void LcdShowCfgCtrParam_Bled_Ctl(void);
void LcdShowCfg_433CO_Off(void);
void LcdShowCfg_433CO_WarningNum(void);
//void LcdShowCfgCtrParam_OilCasl_Full(void);
void Lcd_Show_Manupump(void);
#endif /* INCLUDE_LCD_H_ */
