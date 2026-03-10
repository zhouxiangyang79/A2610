/*
 * lcd.c
 *
 *  Created on: 2024年2月26日
 *  Author: yyz
 */
#include "HAL.h"
#include "tm1628a.h"

const uint8_t num_data[] = {
    0x3f, //"0"
    0x06, //"1"
    0x5b, //"2"
    0x4f, //"3"
    0x66, //"4"
    0x6d, //"5"
    0x7d, //"6"
    0x07, //"7"
    0x7f, //"8"
    0x6f, //"9"
    0x77, //"A"
    0x7c, //"B"
    0x39, //"C"
    0x5e, //"D"
    0x79, //"E"
    0x71, //"F"
    0x76, //"H"
    0x38, //"L"
    0x37, //"N"
    0x3e, //"u"
    0x73, //"P"
    0x5c, //"o"
    0x31, //"r"
    0x6d, //"s"
    0x78, //"t"
    0x50, //"r"
    0x54, //"n"
    0x1c, //"u"
    0x61, //"cc"  ℃
    0x40, //"-"
    0x00, //"熄灭"
    0x04, //点
    0x3d, //"G"
};

/*****************************************************************************
 [函数名称]Lcd_Bk_On()
 [函数功能]打开LCD背光
 *****************************************************************************/
void Lcd_Bk_On(void)
{
    runState.bledDuty = CMD_DISCTRL;
}

/*****************************************************************************
 [函数名称]Lcd_Bk_Off()
 [函数功能]关闭LCD背光
 *****************************************************************************/
void Lcd_Bk_Off(void)
{
    switch(baseParameters.bledDuty)
    {
    case 0:
        runState.bledDuty = 0x80;
        break;
    case 1:
        runState.bledDuty = 0x88;
        break;
    case 2:
        runState.bledDuty = 0x89;
        break;
    case 3:
        runState.bledDuty = 0x8A;
        break;
    case 4:
        runState.bledDuty = 0x8B;
        break;
    case 5:
        runState.bledDuty = 0x8C;
        break;
    case 6:
        runState.bledDuty = 0x8D;
        break;
    case 7:
        runState.bledDuty = 0x8E;
        break;
    case 8:
        runState.bledDuty = CMD_DISCTRL;
        break;
    default:
        runState.bledDuty = CMD_DISCTRL;
        break;
    }
}
/*****************************************************************************
 [函数名称]Hal_Lcd_Init()
 [函数功能]LCD初始化
 *****************************************************************************/
void Hal_Lcd_Init(void)
{
    TM1628A_GpioInit();
    TM1628A_Init();
}

/*****************************************************************************
 [函数名称]Lcd_Set_Segbyid(tm1621_id id, uint8_t bOn)
 [函数功能]LCD按序号段显示
 [参    数] tm1621_id id:显示序号   bOn 置1或0
 [返 回 值]
 [备    注]
 *****************************************************************************/
void Lcd_Set_Segbyid(tm1621_id id, uint8_t bOn)
{
    vSetSeg((id / 9)*2, id % 9, bOn);
}
/*****************************************************************************
 [函数名称]Lcd_Show_Num()
 [函数功能]LCD按数字显示  7段数码管按位设置
 [参    数] u8data:显示的1位数值  addr:显示在哪个数码管上
 *****************************************************************************/
void Lcd_Show_Num(uint8_t u8data, uint8_t addr)
{
    if (u8data < 100) {
        Lcd_Set_Segbyid(addr, num_data[u8data] & 1);              //a
        Lcd_Set_Segbyid(addr + 1, (num_data[u8data] >> 1) & 1);   //b
        Lcd_Set_Segbyid(addr + 2, (num_data[u8data] >> 2) & 1);   //c
        Lcd_Set_Segbyid(addr + 3, (num_data[u8data] >> 3) & 1);   //d
        Lcd_Set_Segbyid(addr + 4, (num_data[u8data] >> 4) & 1);   //e
        Lcd_Set_Segbyid(addr + 5, (num_data[u8data] >> 5) & 1);   //f
        Lcd_Set_Segbyid(addr + 6, (num_data[u8data] >> 6) & 1);   //g
    }
}

/*****************************************************************************
 [函数名称]Lcd_Show_Time(struct tm time)
 [函数功能]LCD显示当前时间
 [参    数]  time 时间   timerstate 定时标志
 *****************************************************************************/
void Lcd_Show_Time(struct tm time,uint8_t timerstate)
{
//    if(timerstate)  Lcd_Set_Segbyid(DD3,1);
//    else            Lcd_Set_Segbyid(DD3,0);

    Lcd_Show_Num(time.tm_hour / 10, A1);
    Lcd_Show_Num(time.tm_hour % 10, A2);
    Lcd_Show_Num(time.tm_min / 10, A3);
    Lcd_Show_Num(time.tm_min % 10, A4);
    uint32_t c = RTC_GetCycle32k();
    c = c / 32950;
    if (c % 2) {
        Lcd_Set_Segbyid(DIO1, 1);
        Lcd_Set_Segbyid(DIO2, 1);
    } else {
        Lcd_Set_Segbyid(DIO1, 0);
        Lcd_Set_Segbyid(DIO2, 0);
    }
}

/*****************************************************************************
 [函数名称]Lcd_Show_LanguageState(uint8_t state)
 [函数功能]LCD显示语音类型
 [参    数] state
 *****************************************************************************/
void Lcd_Show_LanguageState(uint8_t state)
{
//    switch(state)
//    {
//    case SPEECHLANGUARE_OFF:
//        Lcd_Set_Segbyid(S5,0);
//        Lcd_Show_Num(NODISP,A7);
//        break;
//    case SPEECHLANGUARE_CHINESE:
//        Lcd_Set_Segbyid(S5,1);
//        Lcd_Show_Num(C,A7);
//        break;
//    case SPEECHLANGUARE_ENGLISH:
//        Lcd_Set_Segbyid(S5,1);
//        Lcd_Show_Num(E,A7);
//        break;
//    case SPEECHLANGUARE_RUSSIAN:
//        Lcd_Set_Segbyid(S5,1);
//        Lcd_Show_Num(R,A7);
//        break;
//    }
}
/*****************************************************************************
 [函数名称]Lcd_Show_Temp(int16_t temp, uint8_t bON)
 [函数功能]LCD在数码管8、9、10显示温度
 [参    数] temp:温度  bON:闪烁控制(1显示 0关闭) cf:摄氏华氏选择 0摄氏度  1华氏度
 *****************************************************************************/
void Lcd_Show_Temp(int16_t temp, uint8_t bON, uint8_t cf)
{
    float ttemp=0.0;
    if (bON) {
        if(TEMPER_UINT_C == cf)
        {
            Lcd_Set_Segbyid(TC, 1);
            Lcd_Set_Segbyid(TF, 0);

        }else if(TEMPER_UINT_F == cf)
        {
            Lcd_Set_Segbyid(TC, 0);
            Lcd_Set_Segbyid(TF, 1);
            ttemp = ((float)temp*1.8) + 32.0;
            temp = (int16_t)ttemp;
        }
        if (temp >= 0 && temp < 10)
        {
            Lcd_Show_Num(NODISP, A1);
            Lcd_Show_Num(temp % 10, A2);
            Lcd_Show_Num(POINT, A3);
            Lcd_Show_Num(0, A4);
        }
        else if (temp >= 10 && temp < 100) {
            Lcd_Show_Num(temp / 10, A1);
            Lcd_Show_Num(temp % 10, A2);
            Lcd_Show_Num(POINT, A3);
            Lcd_Show_Num(0, A4);
        }else if(temp>=100 && temp<1000)
        {
            Lcd_Show_Num(NODISP, A1);
            Lcd_Show_Num(temp / 100, A2);
            Lcd_Show_Num((temp % 100) / 10, A3);
            Lcd_Show_Num(temp % 10, A4);
        }
        else if (temp > -100 && temp<=-10) {
            Lcd_Show_Num(NODISP, A1);
            Lcd_Show_Num(LINE, A2);
            Lcd_Show_Num((-temp) / 10, A3);
            Lcd_Show_Num((-temp) % 10, A4);
        }
        else if (temp > -10 && temp<0) {
            Lcd_Show_Num(LINE, A1);
            Lcd_Show_Num((-temp) % 10, A2);
            Lcd_Show_Num(POINT, A3);
            Lcd_Show_Num(0, A4);
        }
        else {
            Lcd_Show_Num(LINE, A1);
            Lcd_Show_Num(LINE, A2);
            Lcd_Show_Num(LINE, A3);
            Lcd_Show_Num(LINE, A4);
        }
    } else {
        Lcd_Show_Num(NODISP, A1);
        Lcd_Show_Num(NODISP, A2);
        Lcd_Show_Num(NODISP, A3);
        Lcd_Show_Num(NODISP, A4);
        if(TEMPER_UINT_C == cf)
        {
            Lcd_Set_Segbyid(TC, 1);
            Lcd_Set_Segbyid(TF, 0);
        }else if(TEMPER_UINT_F == cf)
        {
            Lcd_Set_Segbyid(TC, 0);
            Lcd_Set_Segbyid(TF, 1);
        }
    }
}

//设置温度时温度显示
//blink:0不闪烁  1闪烁       cf:摄氏华氏选择
void Lcd_Show_SetTemp(uint8_t blink, uint8_t cf)
{
    static uint8_t blink_state=0;
    static uint16_t last_temp=0;
    uint16_t temp=0;
    float ttemp=0;

//    Lcd_Set_Segbyid(S7 ,1);
//    Lcd_Set_Segbyid(S8 ,0);

    temp = GetRunningSetTemper();

    if(TEMPER_UINT_C == cf)
    {
        Lcd_Set_Segbyid(TC, 1);
        Lcd_Set_Segbyid(TF, 0);

    }else if(TEMPER_UINT_F == cf)
    {
        Lcd_Set_Segbyid(TC, 0);
        Lcd_Set_Segbyid(TF, 1);

        temp = GetRunningSetHTemper();
    }

    if(blink)
    {
        if(last_temp != GetRunningSetTemper())  blink_state = 0;
        if(blink_state<=(DISP_BLINK_FREQUENCY/2))
        {
            Lcd_Show_Num((temp / 10) % 10, A1);
            Lcd_Show_Num(temp % 10, A2);
            Lcd_Show_Num(POINT, A3);
            Lcd_Show_Num(0, A4);
        }else if(blink_state>(DISP_BLINK_FREQUENCY/2))
        {
            Lcd_Show_Num(NODISP, A1);
            Lcd_Show_Num(NODISP, A2);
            Lcd_Show_Num(NODISP, A3);
            Lcd_Show_Num(NODISP, A4);
        }
        if(blink_state++ > DISP_BLINK_FREQUENCY)  blink_state=0;

        last_temp = GetRunningSetTemper();
    }else
    {
        Lcd_Show_Num((temp / 10) % 10, A1);
        Lcd_Show_Num(temp % 10, A2);
        Lcd_Show_Num(POINT, A3);
        Lcd_Show_Num(0, A4);
    }
}
/*****************************************************************************
 [函数名称]Lcd_Show_Level(uint8_t level,uint8_t s48)
 [函数功能]LCD显示档位
 [参    数] level 档位
 [返 回 值]
 [备    注]
 *****************************************************************************/
void Lcd_Show_Level(uint8_t level,uint8_t s48)
{
//    if(s48) Lcd_Set_Segbyid(S48, 1);
    switch (level) {
    case 0:
        Lcd_Set_Segbyid(LV1, 0);
        Lcd_Set_Segbyid(LV2, 0);
        Lcd_Set_Segbyid(LV3, 0);
        Lcd_Set_Segbyid(LV4, 0);
        Lcd_Set_Segbyid(LV5, 0);
        Lcd_Set_Segbyid(LV6, 0);
        Lcd_Set_Segbyid(LV7, 0);
        Lcd_Set_Segbyid(LV8, 0);
        Lcd_Set_Segbyid(LV9, 0);
        Lcd_Set_Segbyid(LV10, 0);
        break;
    case 1:
        Lcd_Set_Segbyid(LV1, 1);
        Lcd_Set_Segbyid(LV2, 0);
        Lcd_Set_Segbyid(LV3, 0);
        Lcd_Set_Segbyid(LV4, 0);
        Lcd_Set_Segbyid(LV5, 0);
        Lcd_Set_Segbyid(LV6, 0);
        Lcd_Set_Segbyid(LV7, 0);
        Lcd_Set_Segbyid(LV8, 0);
        Lcd_Set_Segbyid(LV9, 0);
        Lcd_Set_Segbyid(LV10, 0);
        break;
    case 2:
        Lcd_Set_Segbyid(LV1, 1);
        Lcd_Set_Segbyid(LV2, 1);
        Lcd_Set_Segbyid(LV3, 0);
        Lcd_Set_Segbyid(LV4, 0);
        Lcd_Set_Segbyid(LV5, 0);
        Lcd_Set_Segbyid(LV6, 0);
        Lcd_Set_Segbyid(LV7, 0);
        Lcd_Set_Segbyid(LV8, 0);
        Lcd_Set_Segbyid(LV9, 0);
        Lcd_Set_Segbyid(LV10, 0);
        break;
    case 3:
        Lcd_Set_Segbyid(LV1, 1);
        Lcd_Set_Segbyid(LV2, 1);
        Lcd_Set_Segbyid(LV3, 1);
        Lcd_Set_Segbyid(LV4, 0);
        Lcd_Set_Segbyid(LV5, 0);
        Lcd_Set_Segbyid(LV6, 0);
        Lcd_Set_Segbyid(LV7, 0);
        Lcd_Set_Segbyid(LV8, 0);
        Lcd_Set_Segbyid(LV9, 0);
        Lcd_Set_Segbyid(LV10, 0);
        break;
    case 4:
        Lcd_Set_Segbyid(LV1, 1);
        Lcd_Set_Segbyid(LV2, 1);
        Lcd_Set_Segbyid(LV3, 1);
        Lcd_Set_Segbyid(LV4, 1);
        Lcd_Set_Segbyid(LV5, 0);
        Lcd_Set_Segbyid(LV6, 0);
        Lcd_Set_Segbyid(LV7, 0);
        Lcd_Set_Segbyid(LV8, 0);
        Lcd_Set_Segbyid(LV9, 0);
        Lcd_Set_Segbyid(LV10, 0);
        break;
    case 5:
        Lcd_Set_Segbyid(LV1, 1);
        Lcd_Set_Segbyid(LV2, 1);
        Lcd_Set_Segbyid(LV3, 1);
        Lcd_Set_Segbyid(LV4, 1);
        Lcd_Set_Segbyid(LV5, 1);
        Lcd_Set_Segbyid(LV6, 0);
        Lcd_Set_Segbyid(LV7, 0);
        Lcd_Set_Segbyid(LV8, 0);
        Lcd_Set_Segbyid(LV9, 0);
        Lcd_Set_Segbyid(LV10, 0);
        break;
    case 6:
        Lcd_Set_Segbyid(LV1, 1);
        Lcd_Set_Segbyid(LV2, 1);
        Lcd_Set_Segbyid(LV3, 1);
        Lcd_Set_Segbyid(LV4, 1);
        Lcd_Set_Segbyid(LV5, 1);
        Lcd_Set_Segbyid(LV6, 1);
        Lcd_Set_Segbyid(LV7, 0);
        Lcd_Set_Segbyid(LV8, 0);
        Lcd_Set_Segbyid(LV9, 0);
        Lcd_Set_Segbyid(LV10, 0);
        break;
    case 7:
        Lcd_Set_Segbyid(LV1, 1);
        Lcd_Set_Segbyid(LV2, 1);
        Lcd_Set_Segbyid(LV3, 1);
        Lcd_Set_Segbyid(LV4, 1);
        Lcd_Set_Segbyid(LV5, 1);
        Lcd_Set_Segbyid(LV6, 1);
        Lcd_Set_Segbyid(LV7, 1);
        Lcd_Set_Segbyid(LV8, 0);
        Lcd_Set_Segbyid(LV9, 0);
        Lcd_Set_Segbyid(LV10, 0);
        break;
    case 8:
        Lcd_Set_Segbyid(LV1, 1);
        Lcd_Set_Segbyid(LV2, 1);
        Lcd_Set_Segbyid(LV3, 1);
        Lcd_Set_Segbyid(LV4, 1);
        Lcd_Set_Segbyid(LV5, 1);
        Lcd_Set_Segbyid(LV6, 1);
        Lcd_Set_Segbyid(LV7, 1);
        Lcd_Set_Segbyid(LV8, 1);
        Lcd_Set_Segbyid(LV9, 0);
        Lcd_Set_Segbyid(LV10, 0);
        break;
    case 9:
        Lcd_Set_Segbyid(LV1, 1);
        Lcd_Set_Segbyid(LV2, 1);
        Lcd_Set_Segbyid(LV3, 1);
        Lcd_Set_Segbyid(LV4, 1);
        Lcd_Set_Segbyid(LV5, 1);
        Lcd_Set_Segbyid(LV6, 1);
        Lcd_Set_Segbyid(LV7, 1);
        Lcd_Set_Segbyid(LV8, 1);
        Lcd_Set_Segbyid(LV9, 1);
        Lcd_Set_Segbyid(LV10, 0);
        break;
    case 10:
        Lcd_Set_Segbyid(LV1, 1);
        Lcd_Set_Segbyid(LV2, 1);
        Lcd_Set_Segbyid(LV3, 1);
        Lcd_Set_Segbyid(LV4, 1);
        Lcd_Set_Segbyid(LV5, 1);
        Lcd_Set_Segbyid(LV6, 1);
        Lcd_Set_Segbyid(LV7, 1);
        Lcd_Set_Segbyid(LV8, 1);
        Lcd_Set_Segbyid(LV9, 1);
        Lcd_Set_Segbyid(LV10, 1);
        break;

    default:
        Lcd_Set_Segbyid(LV1, 0);
        Lcd_Set_Segbyid(LV2, 0);
        Lcd_Set_Segbyid(LV3, 0);
        Lcd_Set_Segbyid(LV4, 0);
        Lcd_Set_Segbyid(LV5, 0);
        Lcd_Set_Segbyid(LV6, 0);
        Lcd_Set_Segbyid(LV7, 0);
        Lcd_Set_Segbyid(LV8, 0);
        Lcd_Set_Segbyid(LV9, 0);
        Lcd_Set_Segbyid(LV10, 0);
        break;
    }
}

/*****************************************************************************
 [函数名称]Lcd_Show_PwrOnAnimation(void)
 [函数功能]LCD显示开机动画
 [参    数]
 [返 回 值]0 开机显示完成    1开机进行中
 [备    注]
 *****************************************************************************/
uint8_t Lcd_Show_PwrOnAnimation(void)
{
    static uint8_t level=0;

    vClearScreen();
    if(level==0)    vEV3P035JPlay(TURN_ON_VOICE);
    //显示On
    Lcd_Show_Num(0, A3);
    Lcd_Show_Num(N, A4);
    //显示进度条
    level++;
    Lcd_Show_Level(level,0);
    if(level == 10)
    {
        level = 0;
        return 1;
    }
    else return 0;
}

/*****************************************************************************
 [函数名称]Lcd_Show_PwrOffAnimation(void)
 [函数功能]LCD显示关机动画
 [参    数]
 [返 回 值]0 开机显示完成    1开机进行中
 [备    注]
 *****************************************************************************/
uint8_t Lcd_Show_PwrOffAnimation(void)
{
    static uint8_t level=10;

    vClearScreen();
#if GERMAN==0 || GERMAN==1
    if(level==5)    vEV3P035JPlay(TURN_OFF_VOICE);
#elif GERMAN==2
    if(level==5)    vEV3P035JPlay(STOPPING_HEART);
#endif
    //显示Off
    Lcd_Show_Num(O, A2);
    Lcd_Show_Num(F, A3);
    Lcd_Show_Num(F, A4);
    //显示进度条
    Lcd_Show_Level(level,0);

    if(level-- == 0)
    {
        level = 10;
        return 1;
    }
    else return 0;
}

//等待关机完成
uint8_t Lcd_Show_PwrOffing(void)
{
    vClearScreen();

    if ((RTC_GetCycle32k() / 17000) % 2)
    {
        Lcd_Show_Num(O, A2);
        Lcd_Show_Num(F, A3);
        Lcd_Show_Num(F, A4);
    }
    else
    {
        Lcd_Show_Num(NODISP, A2);
        Lcd_Show_Num(NODISP, A3);
        Lcd_Show_Num(NODISP, A4);
    }

    if(runState.offingBlinkTime-- == 0)
    {
        runState.offingBlinkTime = OFFING_BLINK_TIME;
        if(heaterDatas.runningStep == HEATERRUNSTEP_NULL)  return 1;
        else return 0;

    }
    else return 0;
}

/*****************************************************************************
 [函数名称]Lcd_Show_Airin(uint8_t bON)
 [函数功能]LCD显示进空气
 [参    数] bON 闪烁控制
 [返 回 值]
 [备    注]
 *****************************************************************************/
//void Lcd_Show_Airin(uint8_t bON)
//{
//    static uint8_t cnt = 0; //动态显示变量
//    if (bON) {
//        cnt++;
//        if (cnt / 5 == 0) {
//            Lcd_Set_Segbyid(S15, 1);
//            Lcd_Set_Segbyid(S16, 0);
//            Lcd_Set_Segbyid(S17, 0);
//        }
//        if (cnt / 5 == 1) {
//            Lcd_Set_Segbyid(S15, 1);
//            Lcd_Set_Segbyid(S16, 1);
//            Lcd_Set_Segbyid(S17, 0);
//        }
//        if (cnt / 5 == 2) {
//            Lcd_Set_Segbyid(S15, 1);
//            Lcd_Set_Segbyid(S16, 1);
//            Lcd_Set_Segbyid(S17, 1);
//        }
//        if (cnt > 15) {
//            Lcd_Set_Segbyid(S15, 0);
//            Lcd_Set_Segbyid(S16, 0);
//            Lcd_Set_Segbyid(S17, 0);
//            cnt = 0;
//        }
//    } else {
//        Lcd_Set_Segbyid(S15, 0);
//        Lcd_Set_Segbyid(S16, 0);
//        Lcd_Set_Segbyid(S17, 0);
//    }
//}
/*****************************************************************************
 [函数名称]Lcd_Show_Airout(uint8_t bON)
 [函数功能]LCD显示出空气
 [参    数] bON 闪烁控制
 [返 回 值]
 [备    注]
 *****************************************************************************/
//void Lcd_Show_Airout(uint8_t bON)
//{
//    static uint8_t cnt = 0;
//    if (bON) {
//        cnt++;
//        if (cnt / 5 == 0) {
//            Lcd_Set_Segbyid(S24, 1);
//            Lcd_Set_Segbyid(S25, 0);
//            Lcd_Set_Segbyid(S26, 0);
//        }
//        if (cnt / 5 == 1) {
//            Lcd_Set_Segbyid(S24, 1);
//            Lcd_Set_Segbyid(S25, 1);
//            Lcd_Set_Segbyid(S26, 0);
//        }
//        if (cnt / 5 == 2) {
//            Lcd_Set_Segbyid(S24, 1);
//            Lcd_Set_Segbyid(S25, 1);
//            Lcd_Set_Segbyid(S26, 1);
//        }
//        if (cnt > 15) {
//            Lcd_Set_Segbyid(S24, 0);
//            Lcd_Set_Segbyid(S25, 0);
//            Lcd_Set_Segbyid(S26, 0);
//            cnt = 0;
//        }
//    } else {
//        Lcd_Set_Segbyid(S24, 0);
//        Lcd_Set_Segbyid(S25, 0);
//        Lcd_Set_Segbyid(S26, 0);
//    }
//}
/*****************************************************************************
 [函数名称]Lcd_Show_Fan(uint8_t bON)
 [函数功能]LCD显示风扇
 [参    数] bON 闪烁控制
 [返 回 值]
 [备    注]
 *****************************************************************************/
void Lcd_Show_Fan(uint8_t bON)
{
    if (bON) {
        if ((RTC_GetCycle32k() / 17000) % 2)
        {
            Lcd_Set_Segbyid(FAN1, 1);
            Lcd_Set_Segbyid(FAN2, 1);
            Lcd_Set_Segbyid(FAN3, 1);
        }
        else
        {
            Lcd_Set_Segbyid(FAN1, 0);
            Lcd_Set_Segbyid(FAN2, 0);
            Lcd_Set_Segbyid(FAN3, 0);
        }
    }
    else
    {
        Lcd_Set_Segbyid(FAN1, 1);
        Lcd_Set_Segbyid(FAN2, 1);
        Lcd_Set_Segbyid(FAN3, 1);
    }
}

/*****************************************************************************
 [函数名称]Lcd_Show_Hight(uint8_t bON)
 [函数功能]LCD显示海拔高度
 [参    数] bON:闪烁控制
 [返 回 值]
 [备    注]
 *****************************************************************************/
//void Lcd_Show_Hight(uint16_t height)
//{
//    Lcd_Set_Segbyid(HT,1);
//
//    //显示海拔高度
//    Lcd_Show_Num(height / 10000, A1);
//    Lcd_Show_Num((height % 10000) / 1000, A2);
//    Lcd_Show_Num((height % 1000) / 100, A3);
//    Lcd_Show_Num((height % 100) / 10, A4);
//}

/*****************************************************************************
 [函数名称]Lcd_Show_RemoteMatctState(uint8_t state)
 [函数功能]LCD显示遥控状态
 [参    数]
 [返 回 值]
 [备    注]
 *****************************************************************************/
void Lcd_Show_RemoteMatctState(uint8_t state)
{
    if(GetDisSet433OnOff())
    {
        if (state) {
            Lcd_Set_Segbyid(RF, 1);

        } else {
            Lcd_Set_Segbyid(RF, 0);
        }
    }
}
/*****************************************************************************
 [函数名称]Lcd_Show_BleMatctState(uint8_t state)
 [函数功能]LCD显示蓝牙状态
 [参    数]
 [返 回 值]
 [备    注]
 *****************************************************************************/
void Lcd_Show_BleMatctState(uint8_t state)
{
//    if (state) {
//        Lcd_Set_Segbyid(S15, 1);
//
//    } else {
//        Lcd_Set_Segbyid(S15, 0);
//    }
}

void Lcd_Show_Round(uint8_t bon)
{
    static uint8_t cnt=0;
    Lcd_Set_Segbyid(LG1, 1);
    Lcd_Set_Segbyid(LG2, 1);
    Lcd_Set_Segbyid(LG3, 1);
    Lcd_Set_Segbyid(LG4, 1);

    if (bon)
    {
        if (cnt / 7 == 0) {
            Lcd_Set_Segbyid(IN1, 1);
            Lcd_Set_Segbyid(IN2, 0);
            Lcd_Set_Segbyid(IN3, 0);
            Lcd_Set_Segbyid(OUT1, 0);
            Lcd_Set_Segbyid(OUT2, 0);
            Lcd_Set_Segbyid(OUT3, 0);
        }else if (cnt / 7 == 1) {
            Lcd_Set_Segbyid(IN1, 1);
            Lcd_Set_Segbyid(IN2, 1);
            Lcd_Set_Segbyid(IN3, 0);
            Lcd_Set_Segbyid(OUT1, 0);
            Lcd_Set_Segbyid(OUT2, 0);
            Lcd_Set_Segbyid(OUT3, 0);
        }else if (cnt / 7 == 2) {
            Lcd_Set_Segbyid(IN1, 1);
            Lcd_Set_Segbyid(IN2, 1);
            Lcd_Set_Segbyid(IN3, 1);
            Lcd_Set_Segbyid(OUT1, 0);
            Lcd_Set_Segbyid(OUT2, 0);
            Lcd_Set_Segbyid(OUT3, 0);
        }else if(cnt / 7 == 3){
            Lcd_Set_Segbyid(IN1, 0);
            Lcd_Set_Segbyid(IN2, 0);
            Lcd_Set_Segbyid(IN3, 0);
            Lcd_Set_Segbyid(OUT1, 1);
            Lcd_Set_Segbyid(OUT2, 0);
            Lcd_Set_Segbyid(OUT3, 0);
        }else if(cnt / 7 == 4){
            Lcd_Set_Segbyid(IN1, 0);
            Lcd_Set_Segbyid(IN2, 0);
            Lcd_Set_Segbyid(IN3, 0);
            Lcd_Set_Segbyid(OUT1, 1);
            Lcd_Set_Segbyid(OUT2, 1);
            Lcd_Set_Segbyid(OUT3, 0);
        }else if(cnt / 7 == 5){
            Lcd_Set_Segbyid(IN1, 0);
            Lcd_Set_Segbyid(IN2, 0);
            Lcd_Set_Segbyid(IN3, 0);
            Lcd_Set_Segbyid(OUT1, 1);
            Lcd_Set_Segbyid(OUT2, 1);
            Lcd_Set_Segbyid(OUT3, 1);
        }
        else {
            Lcd_Set_Segbyid(IN1, 0);
            Lcd_Set_Segbyid(IN2, 0);
            Lcd_Set_Segbyid(IN3, 0);
            Lcd_Set_Segbyid(OUT1, 0);
            Lcd_Set_Segbyid(OUT2, 0);
            Lcd_Set_Segbyid(OUT3, 0);
        }
        cnt++;
        if(cnt > 48)   cnt = 0;

    } else {
        Lcd_Set_Segbyid(IN1, 1);
        Lcd_Set_Segbyid(IN2, 1);
        Lcd_Set_Segbyid(IN3, 1);
        Lcd_Set_Segbyid(OUT1, 1);
        Lcd_Set_Segbyid(OUT2, 1);
        Lcd_Set_Segbyid(OUT3, 1);
    }
}

void Lcd_Clear_Round(void)
{
//    Lcd_Set_Segbyid(L1, 0);
//    Lcd_Set_Segbyid(R1, 0);
//    Lcd_Set_Segbyid(L2, 0);
//    Lcd_Set_Segbyid(R2, 0);
//    Lcd_Set_Segbyid(L3, 0);
//    Lcd_Set_Segbyid(R3, 0);
//    Lcd_Set_Segbyid(L4, 0);
//    Lcd_Set_Segbyid(R4, 0);
}

void Lcd_Show_Step(void)
{
    switch(heaterDatas.runningStep)
    {
    case HEATERRUNSTEP_NULL:
        Lcd_Show_Fan(0);
        Lcd_Show_Round(0);
//        if(runState.temperStandbyState == TEMPER_STADNBY_STATE_OFF) Lcd_Clear_Round();  //若是温控关机不显示
//        else Lcd_Show_Round(0);
        break;
    case HEATERRUNSTEP_SELFCHECK://自检
        Lcd_Show_Round(1);
        Lcd_Show_Fan(0);
        break;
    case HEATERRUNSTEP_STARTIGNITE://点火
        Lcd_Show_Round(1);
        Lcd_Show_Fan(0);
        break;
    case HEATERRUNSTEP_BURNING://燃烧
        Lcd_Show_Round(1);
        Lcd_Show_Fan(0);
        break;
    case HEATERRUNSTEP_VENTILATE://通风
        Lcd_Show_Fan(0);
        Lcd_Show_Round(1);
        break;
    default:
        Lcd_Show_Fan(0);
        break;
    case HEATERRUNSTEP_STOP://停机
        break;
    case HEATERRUNSTEP_REST://复位
        break;
    }
}

//运行时显示故障
void Lcd_ShowRunning_ErrCode(void)
{
    if(heaterDatas.faultType != Fault_Null)
    {
        Lcd_Set_Segbyid(WR, 1);
    }else {
        Lcd_Set_Segbyid(WR, 0);
    }
}

//工程模式显示故障
//void Lcd_ShowEngMode_ErrCode(void)
//{
//    Lcd_Show_Num(E, A8);
//    Lcd_Show_Num(heaterDatas.faultType/10, A9);
//    Lcd_Show_Num(heaterDatas.faultType%10, A10);
//}

//温控模式计算运行档位
void Control_RunTemp(void)
{
    int16_t temp=0,cabtemp=0;
    uint8_t level = 0;
    temp = GetRunningSetTemper();  //设定温度
    cabtemp = GetTemperatureValue()/10;
    if(temp>0 && temp>cabtemp)
    {
        level = (temp-cabtemp)/2;
        if(level==0)    level = 1;
    }
    else level = 1;

    SetRunningAutoLevel(level);
}

/*****************************************************************************
 [函数名称]Lcd_ShowRunning(void)
 [函数功能]LCD显示设置运行时间
 [参    数] u8n 数字编号
 [返 回 值]
 [备    注]
 *****************************************************************************/
void Lcd_ShowRunning(void)
{
    uint32_t oillevel=0;
    static uint8_t last_temp=0;
    static uint8_t voice_deviceon=0,voice_deviceoff=0;
    //自动启停开机第一次进入(显示开机动画),进入开机后后面正常显示，同时播放语音

    vClearScreen();

//    Lcd_Show_Hight(heaterDatas.altitude);   //显示海拔高度
    Lcd_Show_RemoteMatctState(GetConfigData_remoteEn());    //显示遥控是否匹配 S13
    //显示当前时间  时、分  是否打开定时
    if(GetDisSetTimerClockEn()) Lcd_Set_Segbyid(CK,1);
    else                        Lcd_Set_Segbyid(CK,0);

    //蓝牙状态
    Lcd_Show_BleMatctState(1);
    //运行步骤显示
    Lcd_Show_Step();

    //运行状态显示
    if(ykq_key==rf_code[2] && ykq_key!=0 && ykq_key!=0xFFFFFFFF) //遥控器模式切换
    {
        ControlModel_Change();
    }else if(ykq_key==rf_code[3] && ykq_key!=0 && ykq_key!=0xFFFFFFFF) //遥控器电源
    {
        vEV3P035JPlay(BE_VOICE);
        TurnOff_Device();
        return;
    }
    if(ykq_key==rf_code[0] || ykq_key==rf_code[1])
    {
        if(ykq_key!=0 && ykq_key!=0xFFFFFFFF)
            vEV3P035JPlay(BE_VOICE);
    }

    switch(GetConfigData_RunMode())
    {
    case CTRL_RUNMODE_TEMP://温度自控
        if(GetDisSetTemperModeStandby())
        {
            if(runState.disSetTempBlinkCnt++ > DISSETTEMPER_STATE_BLINKSET_TIME)
            {
                CleardisSetTempBlinkCnt();
                SetDisSetTemperBlink(0);
                //设定温控模式温度
                SetRunningAutoSetLevel(GetRunningAutoLevel());
            }
            if(ykq_key==rf_code[0] && ykq_key!=0 && ykq_key!=0xFFFFFFFF)
            {
                CleardisSetTempBlinkCnt();
                runState.bklight_time = GetConfigData_BledTime()*10;
                SetDisSetTemperBlink(1);
                RunningSetTemper_Add();
            }else if(ykq_key==rf_code[1] && ykq_key!=0 && ykq_key!=0xFFFFFFFF)
            {
                CleardisSetTempBlinkCnt();
                runState.bklight_time = GetConfigData_BledTime()*10;
                SetDisSetTemperBlink(1);
                RunningSetTemper_Dec();
            }

            if ((RTC_GetCycle32k() / 32950) % 2)
            {
                Lcd_Set_Segbyid(AU ,1);
            }
            else
            {
                Lcd_Set_Segbyid(AU ,0);
            }

            Lcd_Set_Segbyid(LV ,0);
            //根据设定温度和驾驶室温度计算当前档位
            Control_RunTemp();
            Lcd_Show_Level(GetRunningAutoSetLevel(),1);

            //设置温度闪烁是否使能
            if(GetDisSetTemperBlink() == DISSETTEMPER_STATE_BLINKSET)
            {
                Lcd_Show_SetTemp(1,baseParameters.tempUnit);
            }else
            {
//                Lcd_Show_Temp(GetTemperatureValue()/10,1,baseParameters.tempUnit);
                //显示时间
                Lcd_Show_Time(gettimebyoffset(),GetDisSetTimerClockEn());
            }
        }
        else
        {
            if(runState.disSetTempBlinkCnt++ > DISSETTEMPER_STATE_BLINKSET_TIME)
            {
                CleardisSetTempBlinkCnt();
                SetDisSetTemperBlink(0);
                //设定温控模式温度
                SetRunningAutoSetLevel(GetRunningAutoLevel());
            }
            if(ykq_key==rf_code[0] && ykq_key!=0 && ykq_key!=0xFFFFFFFF)
            {
                CleardisSetTempBlinkCnt();
                runState.bklight_time = GetConfigData_BledTime()*10;
                SetDisSetTemperBlink(1);
                RunningSetTemper_Add();
            }else if(ykq_key==rf_code[1] && ykq_key!=0 && ykq_key!=0xFFFFFFFF)
            {
                CleardisSetTempBlinkCnt();
                runState.bklight_time = GetConfigData_BledTime()*10;
                SetDisSetTemperBlink(1);
                RunningSetTemper_Dec();
            }

            Lcd_Set_Segbyid(AU ,1);
            Lcd_Set_Segbyid(LV ,0);

            //根据设定温度和驾驶室温度计算当前档位
            Control_RunTemp();

            Lcd_Show_Level(GetRunningAutoSetLevel(),1);

            //设置温度闪烁是否使能
            if(GetDisSetTemperBlink() == DISSETTEMPER_STATE_BLINKSET)
            {
                Lcd_Show_SetTemp(1,baseParameters.tempUnit);
            }else
            {
//                Lcd_Show_Temp(GetTemperatureValue()/10,1,baseParameters.tempUnit);
                Lcd_Show_Time(gettimebyoffset(),GetDisSetTimerClockEn());
            }
        }
        break;
    case CTRL_RUNMODE_LEVEL://手动
        CleardisSetTempBlinkCnt();
        runState.standbyOffDisCnt = 0;

        if(ykq_key==rf_code[0] && ykq_key!=0 && ykq_key!=0xFFFFFFFF)
        {
            runState.bklight_time = GetConfigData_BledTime()*10;
            RunningSetLevel_Add();
        }else if(ykq_key==rf_code[1] && ykq_key!=0 && ykq_key!=0xFFFFFFFF)
        {
            runState.bklight_time = GetConfigData_BledTime()*10;
            RunningSetLevel_Dec();
        }
        Lcd_Set_Segbyid(AU ,0);
        Lcd_Set_Segbyid(LV ,1);
        Lcd_Show_Level(GetRunningSetLevel(),1);
        Lcd_Show_Time(gettimebyoffset(),GetDisSetTimerClockEn());
        break;
    case CTRL_RUNMODE_VENTILATE:
        CleardisSetTempBlinkCnt();

        runState.standbyOffDisCnt = 0;

        if(ykq_key==rf_code[0] && ykq_key!=0 && ykq_key!=0xFFFFFFFF)
        {
            runState.bklight_time = GetConfigData_BledTime()*10;
            RunningVentilateLevel_Add();
        }else if(ykq_key==rf_code[1] && ykq_key!=0 && ykq_key!=0xFFFFFFFF)
        {
            runState.bklight_time = GetConfigData_BledTime()*10;
            RunningVentilateLevel_Dec();
        }
        Lcd_Set_Segbyid(AU ,0);
        Lcd_Set_Segbyid(LV ,0);
        Lcd_Show_Level(GetRunningVentilateLevel(),1);
        Lcd_Show_Num(NODISP, A1);
        Lcd_Show_Num(U, A2);
        Lcd_Show_Num(E, A3);
        Lcd_Show_Num(N, A4);
//        Lcd_Show_Temp(GetTemperatureValue()/10,1,baseParameters.tempUnit);
        break;
    case CTRL_RUNMODE_FASTHEAT:
        CleardisSetTempBlinkCnt();

        runState.standbyOffDisCnt = 0;
        Lcd_Set_Segbyid(AU ,0);
        Lcd_Set_Segbyid(LV ,0);
        Lcd_Show_Level(10,1);
        Lcd_Show_Num(H, A1);
        Lcd_Show_Num(1, A2);
        Lcd_Show_Num(G, A3);
        Lcd_Show_Num(H, A4);
        break;
    }

    if(GetDisSetTemperModeStandby())    //自动启停打开
    {
        if(runState.temperStandbyState == TEMPER_STADNBY_STATE_OFF)
        {    //自动启停关机时显示
            if(runState.standbyOffDisCnt != 0)
            {
                runState.standbyOffDisCnt++;
                if(runState.standbyOffDisCnt > DISSETTEMPER_STATE_BLINKSET_TIME)
                {
                    runState.standbyOffDisCnt = 0;
                }
            }
            //驾驶室温度闪烁、风机停止、圈清除
            //未收到关机状态，温度一直闪烁显示off
            if(runState.standbyOffDisCnt == 0)
            {
                Lcd_Set_Segbyid(DIO1,0);
                Lcd_Set_Segbyid(DIO2,0);
                if(heaterDatas.runningStep != HEATERRUNSTEP_NULL)
                {
                    if ((RTC_GetCycle32k() / 32950) % 2)
                    {
                        Lcd_Show_Num(NODISP, A1);
                        Lcd_Show_Num(O, A2);
                        Lcd_Show_Num(F, A3);
                        Lcd_Show_Num(F, A4);
                    }
                    else {
                        Lcd_Show_Num(NODISP, A1);
                        Lcd_Show_Num(NODISP, A2);
                        Lcd_Show_Num(NODISP, A3);
                        Lcd_Show_Num(NODISP, A4);
                    }
                }else{
                    //闪烁显示室内温度
                    if ((RTC_GetCycle32k() / 32950) % 2)
                    {
                        Lcd_Show_Temp(GetTemperatureValue()/10,1,baseParameters.tempUnit);
                    }else{
                        Lcd_Show_Temp(GetTemperatureValue()/10,0,baseParameters.tempUnit);
                    }
                    Lcd_Show_Fan(0);
                    Lcd_Clear_Round();
                }
            }
            if(last_temp != GetRunningSetTemper())
            {
                last_temp = GetRunningSetTemper();
                runState.standbyOffDisCnt = 1;
                Lcd_Show_SetTemp(1,baseParameters.tempUnit);
            }
        }else{  //自动启停开机、或者自动启停关闭时显示

        }
    }
    else
    {
        //若是自动启停关机状态下，开机
        if(runState.standbyOnOffDisState == TEMPER_STADNBY_STATE_OFF)
        {
            runState.temperStandbyState = TEMPER_STADNBY_STATE_NULL;
            runState.standbyOnOffDisState = TEMPER_STADNBY_STATE_NULL;
        }
    }

    //运行界面状态下，出现故障码：播放语音，页面跳转
    if(heaterDatas.faultType != Fault_Null)
    {
        //打开背光，页面跳转
        runState.bklight_time = GetConfigData_BledTime()*10;
        switch(heaterDatas.faultType)
        {
        case Fault_Start://启动失败
            vEV3P035JPlay(STARTUP_FAIL);
            break;
        case Fault_Flameout://缺油熄火
            vEV3P035JPlay(LESS_OIL);
            break;
        case Fault_Power://电压异常
            vEV3P035JPlay(VOLTAGE_FAILE);
            break;
        case Fault_OutAirTemp_Senser://出风温度传感器异常
            vEV3P035JPlay(IN_TEP_SENSOR);
            break;
        case Fault_IntAirTemper_Senser://进风温度传感器异常
            vEV3P035JPlay(IN_TEP_SENSOR);
            break;
        case Fault_OilPump:
            vEV3P035JPlay(PUMP_DIS);//油泵断路
            break;
        case Fault_WindMotor://风机异常
            vEV3P035JPlay(FAN_MOTOR);
            break;
        case Fault_Ignition:
            vEV3P035JPlay(IGNITION_FAIL);//预热塞异常
            break;
        case Fault_ShellTemper_Protect://高温保护
            vEV3P035JPlay(OVER_PROTECT);
            break;
        case Fault_ShellTemper_Protect_Senser://高温传感器异常
            vEV3P035JPlay(OVER_TEP_SENSOR);
            break;
        case Fault_LOWBATTERY://电池供电电池电量低
            break;
        }
        //跳转运行故障显示页面
        runState.disIndex = DISINDEX_RUNNING_FAULT;
        runState.voiceIntervalCnt = 1;
        return;
    }

    if(GetConfigData_BledTime()!=0)
    {
        if(runState.bklight_time != 0)
        {
            Lcd_Bk_On();
            runState.bklight_time--;
        }
        else
        {
            Lcd_Bk_Off();
            Lcd_Set_Segbyid(TC ,0);
            Lcd_Set_Segbyid(TF ,0);
            //显示时间
            Lcd_Show_Time(gettimebyoffset(),GetDisSetTimerClockEn());
        }
    }
}

//运行时故障显示
void Lcd_ShowRunning_Fault(void)
{
    vClearScreen();

    if(ykq_key==rf_code[3] && ykq_key!=0 && ykq_key!=0xFFFFFFFF) //遥控器电源键，关机
    {
        TurnOff_Device();
        return;
    }

    //显示故障码
    if(heaterDatas.faultType != Fault_Null)
    {
        Lcd_Show_Num(E, A2);
        if ((RTC_GetCycle32k() / 32950) % 2)
        {
//            Lcd_Show_Num(heaterDatas.faultType/10, A9);
//            Lcd_Show_Num(heaterDatas.faultType%10, A10);
            switch(heaterDatas.faultType)
            {
            case Fault_Power:
                Lcd_Show_Num(0, A3);
                Lcd_Show_Num(1, A4);
                break;
#if EN02==1
            case Fault_LOWBATTERY://电池供电电池电量低
                Lcd_Show_Num(0, A3);
                Lcd_Show_Num(2, A4);
                break;
#endif
            case Fault_Ignition:
                Lcd_Show_Num(0, A3);
                Lcd_Show_Num(3, A4);
                break;
            case Fault_OilPump:
                Lcd_Show_Num(0, A3);
                Lcd_Show_Num(4, A4);
                break;
            case Fault_ShellTemper_Protect:
                Lcd_Show_Num(0, A3);
                Lcd_Show_Num(5, A4);
                break;
            case Fault_WindMotor:
                Lcd_Show_Num(0, A3);
                Lcd_Show_Num(6, A4);
                break;
            case Fault_IntAirTemper_Senser:
                Lcd_Show_Num(0, A3);
                Lcd_Show_Num(7, A4);
                break;
            case Fault_Flameout:
                Lcd_Show_Num(0, A3);
                Lcd_Show_Num(8, A4);
                break;
            case Fault_ShellTemper_Protect_Senser:
                Lcd_Show_Num(0, A3);
                Lcd_Show_Num(9, A4);
                break;
            case Fault_Start:
                Lcd_Show_Num(1, A3);
                Lcd_Show_Num(0, A4);
                break;
            default:
//                Lcd_Show_Num(heaterDatas.faultType/10, A3);
//                Lcd_Show_Num(heaterDatas.faultType%10, A4);
                break;
            }
        }
        else {
            Lcd_Show_Num(NODISP, A3);
            Lcd_Show_Num(NODISP, A4);
        }
    }else{
        TurnOn_FaultToRunning();
    }

    //延时加按键操作判断是否播放语音
    if(runState.voiceIntervalCnt !=0)
    {
        if(runState.voiceIntervalCnt >= VOICE_INTERVAL_CNT)
        {

        }else runState.voiceIntervalCnt++;
    }

    switch(heaterDatas.faultType)
    {
//    case Fault_Null://无故障
//        break;
    case Fault_Start://启动失败
        Lcd_Set_Segbyid(WR, 1);
        if(runState.voiceIntervalCnt == 0)
        {
            vEV3P035JPlay(STARTUP_FAIL);
            runState.voiceIntervalCnt = 1;
        }
        break;
    case Fault_Flameout://缺油熄火
        Lcd_Set_Segbyid(WR, 1);
        if(runState.voiceIntervalCnt == 0)
        {
            vEV3P035JPlay(LESS_OIL);
            runState.voiceIntervalCnt = 1;
        }
        break;
    case Fault_Power://电压异常
        Lcd_Set_Segbyid(WR, 1);
        if(runState.voiceIntervalCnt == 0)
        {
            vEV3P035JPlay(VOLTAGE_FAILE);
            runState.voiceIntervalCnt = 1;
        }
        break;
//    case Fault_OutAirTemp_Senser://出风温度传感器异常
//        Lcd_Set_Segbyid(S11, 1);
//        Lcd_Set_Segbyid(S27, 1);
//        if(runState.voiceIntervalCnt == 0)
//        {
//            vEV3P035JPlay(IN_TEP_SENSOR);
//            runState.voiceIntervalCnt = 1;
//        }
//        break;
//    case Fault_IntAirTemper_Senser://进风温度传感器异常
//        Lcd_Set_Segbyid(S11, 1);
//        Lcd_Set_Segbyid(S14, 1);
//        if(runState.voiceIntervalCnt == 0)
//        {
//            vEV3P035JPlay(IN_TEP_SENSOR);
//            runState.voiceIntervalCnt = 1;
//        }
//        break;
    case Fault_OilPump://油泵断路
        Lcd_Set_Segbyid(WR, 1);
        if(runState.voiceIntervalCnt == 0)
        {
            vEV3P035JPlay(PUMP_DIS);
            runState.voiceIntervalCnt = 1;
        }
        break;
    case Fault_WindMotor://风机异常
        Lcd_Set_Segbyid(WR, 1);
        if(runState.voiceIntervalCnt == 0)
        {
            vEV3P035JPlay(FAN_MOTOR);
            runState.voiceIntervalCnt = 1;
        }
        break;
    case Fault_Ignition://预热塞异常
        Lcd_Set_Segbyid(WR, 1);
        if(runState.voiceIntervalCnt == 0)
        {
            vEV3P035JPlay(IGNITION_FAIL);
            runState.voiceIntervalCnt = 1;
        }
        break;
    case Fault_ShellTemper_Protect://高温保护
        Lcd_Set_Segbyid(WR, 1);
        if(runState.voiceIntervalCnt == 0)
        {
            vEV3P035JPlay(OVER_PROTECT);
            runState.voiceIntervalCnt = 1;
        }
        break;
    case Fault_ShellTemper_Protect_Senser://高温传感器异常
        Lcd_Set_Segbyid(WR, 1);
        if(runState.voiceIntervalCnt == 0)
        {
            vEV3P035JPlay(OVER_TEP_SENSOR);
            runState.voiceIntervalCnt = 1;
        }
        break;
    }

    if(GetConfigData_BledTime()!=0)
    {
        if(runState.bklight_time != 0)
        {
            Lcd_Bk_On();
            runState.bklight_time--;
        }
        else    Lcd_Bk_Off();
    }
}

void Lcd_ShowCO_Value(void)
{
    vClearScreen();

    //运行状态显示
    if(ykq_key==rf_code[3] && ykq_key!=0 && ykq_key!=0xFFFFFFFF) //遥控器电源
    {
        vEV3P035JPlay(BE_VOICE);
        TurnOff_Device();
        return;
    }

    if((GetRunningData_CONumber()/1000)>0)
    {
        Lcd_Show_Num(GetRunningData_CONumber()/1000, A1);
    }else {
        Lcd_Show_Num(NODISP, A1);
    }
    if((GetRunningData_CONumber()%1000/100)>0 || (GetRunningData_CONumber()/1000)!=0)
    {
        Lcd_Show_Num(GetRunningData_CONumber()%1000/100, A2);
    }else {
        Lcd_Show_Num(NODISP, A2);
    }
    if((GetRunningData_CONumber()%100/10)>0 || (GetRunningData_CONumber()/1000)!=0 || (GetRunningData_CONumber()%1000/100)!=0)
    {
        Lcd_Show_Num(GetRunningData_CONumber()%100/10, A3);
    }else {
        Lcd_Show_Num(NODISP, A3);
    }
    Lcd_Show_Num(GetRunningData_CONumber()%10, A4);

    if ((RTC_GetCycle32k() / 18000) % 2)
    {
        Lcd_Set_Segbyid(WR,1);
    }
}

void Lcd_ShowCO_OverWarning(void)
{
    static uint8_t cnt=0;
    vClearScreen();

    if(ykq_key==rf_code[3] && ykq_key!=0 && ykq_key!=0xFFFFFFFF) //遥控器电源键，关机
    {
        TurnOff_Device();
        return;
    }

    Lcd_Bk_On();
    if(runState.coDisTime!=0)
    {
        runState.coDisTime--;
        //显示CO数据
        if((GetRunningData_CONumber()/1000)>0)
        {
            Lcd_Show_Num(GetRunningData_CONumber()/1000, A1);
        }else {
            Lcd_Show_Num(NODISP, A1);
        }
        if((GetRunningData_CONumber()%1000/100)>0 || (GetRunningData_CONumber()/1000)!=0)
        {
            Lcd_Show_Num(GetRunningData_CONumber()%1000/100, A2);
        }else {
            Lcd_Show_Num(NODISP, A2);
        }
        if((GetRunningData_CONumber()%100/10)>0 || (GetRunningData_CONumber()/1000)!=0 || (GetRunningData_CONumber()%1000/100)!=0)
        {
            Lcd_Show_Num(GetRunningData_CONumber()%100/10, A3);
        }else {
            Lcd_Show_Num(NODISP, A3);
        }
        Lcd_Show_Num(GetRunningData_CONumber()%10, A4);

        if ((RTC_GetCycle32k() / 18000) % 2)
        {
            Lcd_Set_Segbyid(WR,1);
        }
    }else
    {
        Lcd_Show_Num(E, A1);
        Lcd_Show_Num(LINE, A2);
        if ((RTC_GetCycle32k() / 32950) % 2)
        {
            Lcd_Show_Num(1, A3);
            Lcd_Show_Num(1, A4);
            cnt++;
            if(cnt>2)
            {
                cnt = 0;
                if(runState.coBeepFlag==1)
                {
                    vEV3P035JPlay(BE_VOICE);
                }
            }
        }
        else
        {
            Lcd_Show_Num(NODISP, A3);
            Lcd_Show_Num(NODISP, A4);
        }
    }
}

//工程模式显示主板版本号
void Lcd_ShowEngMode_HeaterFW(void)
{
    static uint8_t discnt=0;

    vClearScreen();

    Lcd_Show_Level(0,1);
    if(discnt<=15)
    {
        Lcd_Show_Num(u, A2);
        Lcd_Show_Num(heaterDatas.version/10000, A3);
        Lcd_Show_Num((heaterDatas.version/1000)%10, A4);
    }else if(discnt>15)
    {

        Lcd_Show_Num((heaterDatas.version/100)%10, A2);
        Lcd_Show_Num((heaterDatas.version/10)%10, A3);
        Lcd_Show_Num(heaterDatas.version%10, A4);
    }
    if(discnt++ > 30)  discnt=0;
}

//故障码
void Lcd_ShowEngMode_FaultType(void)
{
    vClearScreen();

    Lcd_Show_Level(1,1);
    Lcd_Show_Num(E, A2);
    switch(heaterDatas.faultType)
    {
    case Fault_Null:
        Lcd_Show_Num(0, A3);
        Lcd_Show_Num(0, A4);
        break;
    case Fault_Power://电源
        Lcd_Show_Num(0, A3);
        Lcd_Show_Num(1, A4);
        break;
    case Fault_Ignition://加热塞
        Lcd_Show_Num(0, A3);
        Lcd_Show_Num(3, A4);
        break;
    case Fault_OilPump://油泵
        Lcd_Show_Num(0, A3);
        Lcd_Show_Num(4, A4);
        break;
    case Fault_ShellTemper_Protect://高温保护
        Lcd_Show_Num(0, A3);
        Lcd_Show_Num(5, A4);
        break;
    case Fault_WindMotor://风机
        Lcd_Show_Num(0, A3);
        Lcd_Show_Num(6, A4);
        break;
    case Fault_IntAirTemper_Senser:
        Lcd_Show_Num(0, A3);
        Lcd_Show_Num(7, A4);
        break;
    case Fault_Flameout:
        Lcd_Show_Num(0, A3);
        Lcd_Show_Num(8, A4);
        break;
    case Fault_ShellTemper_Protect_Senser:
        Lcd_Show_Num(0, A3);
        Lcd_Show_Num(9, A4);
        break;
    case Fault_Start://缺油熄火
        Lcd_Show_Num(1, A3);
        Lcd_Show_Num(0, A4);
        break;
    default:
//        Lcd_Show_Num(heaterDatas.faultType/10, A3);
//        Lcd_Show_Num(heaterDatas.faultType%10, A4);
        break;
    }
//
//    Lcd_Show_Num((heaterDatas.faultType/10)%10, A9);
//    Lcd_Show_Num(heaterDatas.faultType%10, A10);
}

//壳体温度
void Lcd_ShowEngMode_ShellTemper(void)
{
    vClearScreen();

    Lcd_Show_Level(2,1);
    Lcd_Show_Temp(heaterDatas.shellTemp/10,1,baseParameters.tempUnit);
}

//供电电压
void Lcd_ShowEngMode_SupVol(void)
{
    vClearScreen();

    Lcd_Show_Level(3,1);
    Lcd_Show_Num(heaterDatas.supplyVoltage/100, A3);
    Lcd_Show_Num((heaterDatas.supplyVoltage%100)/10, A4);
}

//加热器档位
void Lcd_ShowEngMode_RunLevel(void)
{
    vClearScreen();

    Lcd_Show_Level(4,1);
    if(GetHeaterDatas_RunningLevel()/10)
    {
        Lcd_Show_Num(GetHeaterDatas_RunningLevel()/10, A2);
    }
    Lcd_Show_Num(GetHeaterDatas_RunningLevel()%10, A3);
    Lcd_Show_Num(L, A4);
}

//驾驶室温度显示
void Lcd_ShowEngMode_RoomTemp(void)
{
    vClearScreen();

    Lcd_Show_Level(5,1);
    Lcd_Show_Temp(GetTemperatureValue()/10,1,baseParameters.tempUnit);
}

//海拔高度显示
void Lcd_ShowEngMode_Altitude(void)
{
    vClearScreen();

    Lcd_Show_Level(6,1);
    Lcd_Set_Segbyid(HT, 1);
    Lcd_Set_Segbyid(M, 1);

    //显示海拔高度
    if(heaterDatas.altitude==0)
    {
        if(heaterDatas.cabAltitude==0xFFFF)
        {
            Lcd_Show_Num(LINE, A1);
            Lcd_Show_Num(LINE, A2);
            Lcd_Show_Num(LINE, A3);
            Lcd_Show_Num(LINE, A4);
        }
        else
        {
            Lcd_Show_Num(heaterDatas.cabAltitude / 1000, A1);
            Lcd_Show_Num((heaterDatas.cabAltitude % 1000) / 100, A2);
            Lcd_Show_Num((heaterDatas.cabAltitude % 100) / 10, A3);
            Lcd_Show_Num(heaterDatas.cabAltitude % 10, A4);
        }
    }else
    {
        Lcd_Show_Num(heaterDatas.altitude / 10000, A1);
        Lcd_Show_Num((heaterDatas.altitude % 10000) / 1000, A2);
        Lcd_Show_Num((heaterDatas.altitude % 1000) / 100, A3);
        Lcd_Show_Num((heaterDatas.altitude % 100) / 10, A4);
    }
}

void Lcd_ShowEngMode_ManUpUmp(void)
{
    vClearScreen();

    Lcd_Show_Level(7,1);
    Lcd_Show_Num(LINE, A2);
    Lcd_Show_Num(P, A3);
    Lcd_Show_Num(LINE, A4);
}

//添加遥控
void Lcd_ShowEngMode_RemoteMatch(void)
{
    static uint8_t delayoff=0;
    vClearScreen();

    Lcd_Show_Level(8,1);
    Lcd_Set_Segbyid(RF,1);

    switch(runState.remoteMatchState)
    {
    case DISSETENGMODE_REMOTEMATCH_NULL:
        Lcd_Show_Num(r, A2);
        Lcd_Show_Num(T, A3);
        Lcd_Show_Num(E, A4);
        break;
    case DISSETENGMODE_REMOTEMATCH_KEYADD:
        Lcd_Show_Num(P, A3);
        Lcd_Show_Num(1, A4);
        break;
    case DISSETENGMODE_REMOTEMATCH_KEYDEC:
        Lcd_Show_Num(P, A3);
        Lcd_Show_Num(2, A4);
        break;
    case DISSETENGMODE_REMOTEMATCH_KEYMODE:
        Lcd_Show_Num(P, A3);
        Lcd_Show_Num(3, A4);
        break;
    case DISSETENGMODE_REMOTEMATCH_KEYPOWER:
        Lcd_Show_Num(P, A3);
        Lcd_Show_Num(4, A4);
        break;
    case DISSETENGMODE_REMOTEMATCH_CLEARSUC:
            Lcd_Show_Num(S, A2);
            Lcd_Show_Num(U, A3);
            Lcd_Show_Num(C, A4);
            delayoff ++;
            if(delayoff>20)
            {
                delayoff = 0;
                TurnOff_DeviceNow();
            }
            break;
    default:
        break;
    }
    if (ykq_key!=0 && ykq_key!=0xFFFFFFFF && ykq_key != ykq_key_last && runState.remoteMatchState!=DISSETENGMODE_REMOTEMATCH_NULL)
    {
        rf_code[DISSETENGMODE_REMOTEMATCH_KEYADD-1] = (ykq_key&0xFFFFFFF0)|0x00000008;      //+
        rf_code[DISSETENGMODE_REMOTEMATCH_KEYDEC-1] = (ykq_key&0xFFFFFFF0)|0x00000002;      //-
        rf_code[DISSETENGMODE_REMOTEMATCH_KEYMODE-1] = (ykq_key&0xFFFFFFF0)|0x00000001;     //On/Mode
        rf_code[DISSETENGMODE_REMOTEMATCH_KEYPOWER-1] = (ykq_key&0xFFFFFFF0)|0x00000004;    //Off
        baseParameters.remoteEn = TRUE;
        baseParameters.rf433OnOff = TRUE;
        vPARA_SAVE();
        vClearScreen();
        runState.disSetTimeState = DISSETTIME_STATE_NULL;
        runState.remoteMatchState = DISSETENGMODE_REMOTEMATCH_NULL;
        ClearDisSetTimeBlinkCnt();
        //不在关机快捷添加遥控界面
        if(runState.runModel != REMOTE_STATE_POWER_OFF)
        {
            runState.disIndex = DISINDEX_RUNNING;
            runState.bklight_time = GetConfigData_BledTime()*10;
        }else{
            TurnOff_DeviceNow();
        }
    }
}

extern uint8_t attDeviceName[GAP_DEVICE_NAME_LEN];
//蓝牙匹配
void Lcd_ShowEngMode_BleMatch(void)
{
    static uint8_t cnt=0;
    vClearScreen();

    Lcd_Show_Level(9,1);
    switch(runState.disSetTimeState)
    {
    case DISSETTIME_STATE_NULL:
        cnt =0;
        //显示蓝牙后4位
        if(attDeviceName[12]<58)
            Lcd_Show_Num(attDeviceName[12]-0x30, A1);
        else
            Lcd_Show_Num(attDeviceName[12]-55, A1);
        if(attDeviceName[13]<58)
            Lcd_Show_Num(attDeviceName[13]-0x30, A2);
        else
            Lcd_Show_Num(attDeviceName[13]-55, A2);
        if(attDeviceName[14]<58)
            Lcd_Show_Num(attDeviceName[14]-0x30, A3);
        else
            Lcd_Show_Num(attDeviceName[14]-55, A3);
        if(attDeviceName[15]<58)
            Lcd_Show_Num(attDeviceName[15]-0x30, A4);
        else
            Lcd_Show_Num(attDeviceName[15]-55, A4);
//        Lcd_Show_Num(B, A8);
//        Lcd_Show_Num(L, A9);
//        Lcd_Show_Num(E, A10);
        break;
    case DISSETTIME_STATE_HOUR_MINUTE:
        cnt =0;
        //闪烁显示密码
        if(runState.disSetTimeBlinkCnt<=(DISP_BLINK_FREQUENCY/2))
        {
            Lcd_Show_Num(baseParameters.blePassword/256/10, A1);
            Lcd_Show_Num(baseParameters.blePassword/256%10, A2);
            Lcd_Show_Num(baseParameters.blePassword%256/10, A3);
            Lcd_Show_Num(baseParameters.blePassword%256%10, A4);
        }else if(runState.disSetTimeBlinkCnt>(DISP_BLINK_FREQUENCY/2))
        {
            Lcd_Show_Num(NODISP, A1);
            Lcd_Show_Num(NODISP, A2);
            Lcd_Show_Num(NODISP, A3);
            Lcd_Show_Num(NODISP, A4);
        }

//        Lcd_Show_Num(B, A8);
//        Lcd_Show_Num(L, A9);
//        Lcd_Show_Num(E, A10);
        break;
    case DISSETTIME_STATE_HOUR://密码匹配成功，显示SUC
//        Lcd_Show_Num(baseParameters.blePassword/256/10, A1);
//        Lcd_Show_Num(baseParameters.blePassword/256%10, A2);
//        Lcd_Show_Num(baseParameters.blePassword%256/10, A5);
//        Lcd_Show_Num(baseParameters.blePassword%256%10, A6);

        Lcd_Show_Num(S, A1);
        Lcd_Show_Num(U, A2);
        Lcd_Show_Num(C, A3);
        if(cnt++ > 25)
        {
            cnt = 0;
            runState.disSetTimeState = DISSETTIME_STATE_NULL;
            runState.disIndex = DISINDEX_RUNNING;
            runState.bklight_time = GetConfigData_BledTime()*10;
        }
        break;
    }
//    Lcd_Set_Segbyid(S15,1);
    runState.disSetTimeBlinkCnt ++;
    if(runState.disSetTimeBlinkCnt>DISP_BLINK_FREQUENCY) runState.disSetTimeBlinkCnt=0;
}

void Lcd_ShowEngMode_Rest(void)
{
    vClearScreen();

    Lcd_Show_Level(10,1);
    switch(runState.disSetTimeState)
    {
    case DISSETTIME_STATE_NULL:
        Lcd_Show_Num(R, A1);
        Lcd_Show_Num(E, A2);
        Lcd_Show_Num(S, A3);
        Lcd_Show_Num(T, A4);
        break;
    }
}

//显示Vcc电压采集变化率
#if VCC_CHICK==1
void Lcd_ShowEngMode_Vcc(void)
{
    static uint8_t cnt=0;
    vClearScreen();
    runState.vccAdcDiffer = runState.vccAdcMax - runState.vccAdcMin;

    switch(runState.disSetTimeState)
    {
    case DISSETTIME_STATE_NULL://差
        Lcd_Show_Num(runState.vccAdcDiffer/1000, A1);
        Lcd_Show_Num(runState.vccAdcDiffer%1000/100, A2);
        Lcd_Show_Num(runState.vccAdcDiffer%100/10, A3);
        Lcd_Show_Num(runState.vccAdcDiffer%10, A4);
        break;
    case DISSETTIME_STATE_HOUR://最大值
        Lcd_Show_Num(runState.vccAdcMax/1000, A1);
        Lcd_Show_Num(runState.vccAdcMax%1000/100, A2);
        Lcd_Show_Num(runState.vccAdcMax%100/10, A3);
        Lcd_Show_Num(runState.vccAdcMax%10, A4);
        break;
    case DISSETTIME_STATE_MINUTE://最小值
        Lcd_Show_Num(runState.vccAdcMin/1000, A1);
        Lcd_Show_Num(runState.vccAdcMin%1000/100, A2);
        Lcd_Show_Num(runState.vccAdcMin%100/10, A3);
        Lcd_Show_Num(runState.vccAdcMin%10, A4);
        break;
    }


    cnt++;
    if(cnt>=10)
    {
        cnt=0;
        runState.vccAdcMax = 2500;
        runState.vccAdcMin = 3145;
    }
}
#endif

//基本参数时间设置
void LcdShowCfgCtrParam_RTC(void)
{
    baseParameters.sttime = gettimebyoffset();
    vClearScreen();

//    Lcd_Set_Segbyid(S4, 1);
    switch(GetDisSetTimeState())
    {
    case DISSETTIME_STATE_NULL:
//        Lcd_Show_Num(F, A1);
//        Lcd_Show_Num(1, A2);
        Lcd_Show_Num(baseParameters.sttime.tm_hour / 10, A1);
        Lcd_Show_Num(baseParameters.sttime.tm_hour % 10, A2);
        Lcd_Set_Segbyid(DIO1,1);
        Lcd_Set_Segbyid(DIO2,1);
        Lcd_Show_Num(baseParameters.sttime.tm_min / 10, A3);
        Lcd_Show_Num(baseParameters.sttime.tm_min % 10, A4);
        break;
    case DISSETTIME_STATE_HOUR:
        Lcd_Set_Segbyid(DIO1,1);
        Lcd_Set_Segbyid(DIO2,1);
//        if(EC11_NOKEY != ec11_key)
//        {
//            ClearDisSetTimeBlinkCnt();
//            if(EC11_ADD == ec11_key)
//            {
//                SetDisSetTimeHourAdd();
//            }else if(EC11_DEC == ec11_key)
//            {
//                SetDisSetTimeHourDec();
//            }
//        }
        if(runState.disSetTimeBlinkCnt<=(DISP_BLINK_FREQUENCY/2))
        {
            Lcd_Show_Num(baseParameters.sttime.tm_hour / 10, A1);
            Lcd_Show_Num(baseParameters.sttime.tm_hour % 10, A2);

        }else if(runState.disSetTimeBlinkCnt>(DISP_BLINK_FREQUENCY/2))
        {
            Lcd_Show_Num(NODISP, A3);
            Lcd_Show_Num(NODISP, A4);
        }
        Lcd_Show_Num(baseParameters.sttime.tm_min / 10, A3);
        Lcd_Show_Num(baseParameters.sttime.tm_min % 10, A4);
        runState.disSetTimeBlinkCnt ++;
        break;
    case DISSETTIME_STATE_MINUTE:
        Lcd_Set_Segbyid(DIO1,1);
        Lcd_Set_Segbyid(DIO2,1);
//        if(EC11_NOKEY != ec11_key)
//        {
//            ClearDisSetTimeBlinkCnt();
//            if(EC11_ADD == ec11_key)
//            {
//                SetDisSetTimeMinuteAdd();
//            }else if(EC11_DEC == ec11_key)
//            {
//                SetDisSetTimeMinuteDec();
//            }
//        }
        if(runState.disSetTimeBlinkCnt<=(DISP_BLINK_FREQUENCY/2))
        {
            Lcd_Show_Num(baseParameters.sttime.tm_min / 10, A3);
            Lcd_Show_Num(baseParameters.sttime.tm_min % 10, A4);
        }else if(runState.disSetTimeBlinkCnt>(DISP_BLINK_FREQUENCY/2))
        {
            Lcd_Show_Num(NODISP, A3);
            Lcd_Show_Num(NODISP, A4);
        }
        Lcd_Show_Num(baseParameters.sttime.tm_hour / 10, A1);
        Lcd_Show_Num(baseParameters.sttime.tm_hour % 10, A2);
        runState.disSetTimeBlinkCnt ++;
        break;
    default:
        break;
    }
    if(runState.disSetTimeBlinkCnt>DISP_BLINK_FREQUENCY) runState.disSetTimeBlinkCnt=0;
}

//定时时间
void LcdShowCfgCtrParam_Timer(void)
{
    vClearScreen();
    Lcd_Set_Segbyid(DIO1,1);
    Lcd_Set_Segbyid(DIO2,1);
    Lcd_Set_Segbyid(CK,1);
    switch(GetDisSetTimeState())
    {
    case DISSETTIME_STATE_NULL:
        Lcd_Show_Num(baseParameters.timerClockHour / 10, A1);
        Lcd_Show_Num(baseParameters.timerClockHour % 10, A2);
        Lcd_Show_Num(baseParameters.timerClockMinute / 10, A3);
        Lcd_Show_Num(baseParameters.timerClockMinute % 10, A4);
        break;
    case DISSETTIME_STATE_HOUR:

        if(runState.disSetTimeBlinkCnt<=(DISP_BLINK_FREQUENCY/2))
        {
            Lcd_Show_Num(baseParameters.timerClockHour / 10, A1);
            Lcd_Show_Num(baseParameters.timerClockHour % 10, A2);

        }else if(runState.disSetTimeBlinkCnt>(DISP_BLINK_FREQUENCY/2))
        {
            Lcd_Show_Num(NODISP, A1);
            Lcd_Show_Num(NODISP, A2);
        }
        Lcd_Show_Num(baseParameters.timerClockMinute / 10, A3);
        Lcd_Show_Num(baseParameters.timerClockMinute % 10, A4);
        runState.disSetTimeBlinkCnt ++;
        break;
    case DISSETTIME_STATE_MINUTE:
        if(runState.disSetTimeBlinkCnt<=(DISP_BLINK_FREQUENCY/2))
        {
            Lcd_Show_Num(baseParameters.timerClockMinute / 10, A3);
            Lcd_Show_Num(baseParameters.timerClockMinute % 10, A4);

        }else if(runState.disSetTimeBlinkCnt>(DISP_BLINK_FREQUENCY/2))
        {
            Lcd_Show_Num(NODISP, A3);
            Lcd_Show_Num(NODISP, A4);
        }
        Lcd_Show_Num(baseParameters.timerClockHour / 10, A1);
        Lcd_Show_Num(baseParameters.timerClockHour % 10, A2);
        runState.disSetTimeBlinkCnt ++;
        break;
    }
    if(runState.disSetTimeBlinkCnt>DISP_BLINK_FREQUENCY) runState.disSetTimeBlinkCnt=0;
}

//定时开机运行时间
void LcdShowCfgCtrParam_Timer_RunTime(void)
{
    vClearScreen();
    Lcd_Set_Segbyid(DIO2,1);
    switch(GetDisSetTimeState())
    {
    case DISSETTIME_STATE_NULL:
        Lcd_Show_Num(GetDisSetTimerRunTime() / 600, A1);
        Lcd_Show_Num((GetDisSetTimerRunTime() % 600) / 60, A2);
        Lcd_Show_Num((GetDisSetTimerRunTime() % 60) / 10, A3);
        Lcd_Show_Num((GetDisSetTimerRunTime() % 60) % 10, A4);
        break;
    case DISSETTIME_STATE_HOUR_MINUTE:
        if(runState.disSetTimeBlinkCnt<=(DISP_BLINK_FREQUENCY/2))
        {
            Lcd_Show_Num(GetDisSetTimerRunTime() / 600, A1);
            Lcd_Show_Num((GetDisSetTimerRunTime() % 600) / 60, A2);
            Lcd_Show_Num((GetDisSetTimerRunTime() % 60) / 10, A3);
            Lcd_Show_Num((GetDisSetTimerRunTime() % 60) % 10, A4);

        }else if(runState.disSetTimeBlinkCnt>(DISP_BLINK_FREQUENCY/2))
        {
            Lcd_Show_Num(NODISP, A1);
            Lcd_Show_Num(NODISP, A2);
            Lcd_Show_Num(NODISP, A3);
            Lcd_Show_Num(NODISP, A4);
        }
        runState.disSetTimeBlinkCnt ++;
        break;
    }
    if(runState.disSetTimeBlinkCnt>DISP_BLINK_FREQUENCY) runState.disSetTimeBlinkCnt=0;
}

//定时使能
void LcdShowCfgCtrParam_Timer_Sw(void)
{
    vClearScreen();
    Lcd_Show_Num(T, A1);
    Lcd_Show_Num(LINE, A2);

    switch(GetDisSetTimeState())
    {
    case DISSETTIME_STATE_NULL:
        if(GetDisSetTimerClockEn())
        {
            Lcd_Show_Num(O, A3);
            Lcd_Show_Num(N, A4);
        }else
        {
            Lcd_Show_Num(O, A3);
            Lcd_Show_Num(F, A4);
        }
        break;
    case DISSETTIME_STATE_MINUTE:
        if(runState.disSetTimeBlinkCnt<=(DISP_BLINK_FREQUENCY/2))
        {
            if(GetDisSetTimerClockEn())
            {
                Lcd_Show_Num(O, A3);
                Lcd_Show_Num(N, A4);
            }else
            {
                Lcd_Show_Num(O, A3);
                Lcd_Show_Num(F, A4);
            }
        }else if(runState.disSetTimeBlinkCnt>(DISP_BLINK_FREQUENCY/2))
        {
            Lcd_Show_Num(NODISP, A3);
            Lcd_Show_Num(NODISP, A4);
        }
        runState.disSetTimeBlinkCnt ++;
        break;
    }
    if(runState.disSetTimeBlinkCnt>DISP_BLINK_FREQUENCY) runState.disSetTimeBlinkCnt=0;
}

//语音选择
void LcdShowCfgCtrParam_Voice_Sw(void)
{
    static uint8_t cnt=0;
    vClearScreen();
    Lcd_Show_Num(u, A1);
    Lcd_Show_Num(LINE, A2);
//    Lcd_Set_Segbyid(DIO1,1);
//    Lcd_Set_Segbyid(DIO2,1);

    switch(GetDisSetLanguageType())
    {
    case SPEECHLANGUARE_OFF:
        Lcd_Show_Num(O, A3);
        Lcd_Show_Num(F, A4);
        break;
#if GERMAN==0 || GERMAN==1
    case SPEECHLANGUARE_ENGLISH:
        Lcd_Show_Num(E, A3);
        Lcd_Show_Num(N, A4);
        break;
    case SPEECHLANGUARE_CHINESE:
        Lcd_Show_Num(G, A3);
        Lcd_Show_Num(E, A4);
        break;
    case SPEECHLANGUARE_RUSSIAN:
        Lcd_Show_Num(R, A3);
        Lcd_Show_Num(U, A4);
        break;
#elif GERMAN==2
    case SPEECHLANGUARE_ENGLISH:
        Lcd_Show_Num(T, A3);
        Lcd_Show_Num(r, A4);
        break;
#endif
    }

    cnt++;
    if(cnt>15)
    {
        cnt = 0;
        vPARA_SAVE();
        runState.disSetTimeState = DISSETTIME_STATE_NULL;
        ClearDisSetTimeBlinkCnt();
        runState.disIndex = DISINDEX_RUNNING;
        runState.bklight_time = GetConfigData_BledTime()*10;
    }
}


void LcdShowOffSet_Temper(int8_t temp,uint8_t bon)
{
    Lcd_Set_Segbyid(TC,1);
    if(bon)
    {
        if(temp>=0 && temp<10)
        {
            Lcd_Show_Num(temp, A4);
        }else if(temp<0 && temp>-10)
        {
            Lcd_Show_Num(LINE, A3);
            Lcd_Show_Num(-temp, A4);
        }else{
            Lcd_Show_Num(LINE, A3);
            Lcd_Show_Num(LINE, A4);
        }
    }else
    {
        Lcd_Show_Num(NODISP, A3);
        Lcd_Show_Num(NODISP, A4);
    }
}

//热敏电阻偏差设置
void LcdShowCfgCtrParam_TemperOffSet(void)
{
    vClearScreen();
//    Lcd_Show_Num(F, A1);
//    Lcd_Show_Num(5, A2);
//    Lcd_Set_Segbyid(DIO1,1);
//    Lcd_Set_Segbyid(DIO2,1);

    switch(GetDisSetTimeState())
    {
    case DISSETTIME_STATE_NULL:
        LcdShowOffSet_Temper(GetDisSetTemperOffSet(),1);
        break;
    case DISSETTIME_STATE_HOUR_MINUTE:
        if(runState.disSetTimeBlinkCnt<=(DISP_BLINK_FREQUENCY/2))
        {
            LcdShowOffSet_Temper(GetDisSetTemperOffSet(),1);
        }else if(runState.disSetTimeBlinkCnt>(DISP_BLINK_FREQUENCY/2))
        {
            LcdShowOffSet_Temper(GetDisSetTemperOffSet(),0);
        }
        runState.disSetTimeBlinkCnt ++;
        break;
    }
    if(runState.disSetTimeBlinkCnt>DISP_BLINK_FREQUENCY) runState.disSetTimeBlinkCnt=0;
}

//油箱大小显示
//void LcdShowCfgCtrParam_Oilcasl_Volume(void)
//{
//    vClearScreen();
//    Lcd_Show_Num(F, A1);
//    Lcd_Show_Num(6, A2);
//    Lcd_Set_Segbyid(DIO1,1);
//    Lcd_Set_Segbyid(DIO2,1);
//    switch(GetDisSetTimeState())
//    {
//    case DISSETTIME_STATE_NULL:
//       Lcd_Show_Num(GetDisSetOilCaseVolume()/10, A3);
//       Lcd_Show_Num(GetDisSetOilCaseVolume()%10, A4);
//       break;
//    case DISSETTIME_STATE_HOUR_MINUTE:
//       if(runState.disSetTimeBlinkCnt<=(DISP_BLINK_FREQUENCY/2))
//       {
//           if(GetDisSetOilCaseVolume() == 0)
//          {
//               Lcd_Show_Num(LINE, A3);
//               Lcd_Show_Num(LINE, A4);
//          }else
//          {
//              Lcd_Show_Num(GetDisSetOilCaseVolume()/10, A3);
//              Lcd_Show_Num(GetDisSetOilCaseVolume()%10, A4);
//          }
//       }else if(runState.disSetTimeBlinkCnt>(DISP_BLINK_FREQUENCY/2))
//       {
//           Lcd_Show_Num(NODISP, A3);
//           Lcd_Show_Num(NODISP, A4);
//       }
//       runState.disSetTimeBlinkCnt ++;
//       break;
//   }
//   if(runState.disSetTimeBlinkCnt>DISP_BLINK_FREQUENCY) runState.disSetTimeBlinkCnt=0;
//}

//显示油泵类型   16ul  22ul  28ul  32ul
//void LcdShow_Oilpump_Type(uint8_t type)
//{
//    switch(type)
//    {
//    case OILPUMP_TYPE_16UL:
//        Lcd_Show_Num(1,A3);
//        Lcd_Show_Num(6,A4);
//        break;
//    case OILPUMP_TYPE_NULL:
//    case OILPUMP_TYPE_22UL:
//        Lcd_Show_Num(2,A3);
//        Lcd_Show_Num(2,A4);
//        break;
//    case OILPUMP_TYPE_28UL:
//        Lcd_Show_Num(2,A3);
//        Lcd_Show_Num(8,A4);
//        break;
//    case OILPUMP_TYPE_32UL:
//        Lcd_Show_Num(3,A3);
//        Lcd_Show_Num(2,A4);
//        break;
//    default:
//        break;
//    }
//}

//void LcdShowCfgCtrParam_Oilpump_Type(void)
//{
//    vClearScreen();
//    Lcd_Show_Num(F, A1);
//    Lcd_Show_Num(7, A2);
//    Lcd_Set_Segbyid(DIO1,1);
//    Lcd_Set_Segbyid(DIO2,1);
//    switch(GetDisSetTimeState())
//    {
//    case DISSETTIME_STATE_NULL:
//        LcdShow_Oilpump_Type(GetDisSetOilPumpType());
//        break;
//    case DISSETTIME_STATE_HOUR_MINUTE:
//       if(runState.disSetTimeBlinkCnt<=(DISP_BLINK_FREQUENCY/2))
//       {
//           LcdShow_Oilpump_Type(GetDisSetOilPumpType());
//       }else if(runState.disSetTimeBlinkCnt>(DISP_BLINK_FREQUENCY/2))
//       {
//           Lcd_Show_Num(NODISP, A3);
//           Lcd_Show_Num(NODISP, A4);
//       }
//       runState.disSetTimeBlinkCnt ++;
//       break;
//    }
//    if(runState.disSetTimeBlinkCnt>DISP_BLINK_FREQUENCY) runState.disSetTimeBlinkCnt=0;
//}

void LcdShowCfgCtrParam_BLEOnOff(void)
{
    vClearScreen();
    Lcd_Show_Num(B, A1);
    Lcd_Show_Num(LINE, A2);
//    Lcd_Set_Segbyid(DIO1,1);
//    Lcd_Set_Segbyid(DIO2,1);

    switch(GetDisSetTimeState())
    {
    case DISSETTIME_STATE_NULL:
        if(GetDisSetBleOnOff())
        {
            Lcd_Show_Num(O, A3);
            Lcd_Show_Num(N, A4);
        }else
        {
            Lcd_Show_Num(O, A3);
            Lcd_Show_Num(F, A4);
        }
        break;
    case DISSETTIME_STATE_MINUTE:
        if(runState.disSetTimeBlinkCnt<=(DISP_BLINK_FREQUENCY/2))
        {
            if(GetDisSetBleOnOff())
            {
                Lcd_Show_Num(O, A3);
                Lcd_Show_Num(N, A4);
            }else
            {
                Lcd_Show_Num(O, A3);
                Lcd_Show_Num(F, A4);
            }
        }else if(runState.disSetTimeBlinkCnt>(DISP_BLINK_FREQUENCY/2))
        {
            Lcd_Show_Num(NODISP, A3);
            Lcd_Show_Num(NODISP, A4);
        }
        runState.disSetTimeBlinkCnt ++;
        break;
    }
    if(runState.disSetTimeBlinkCnt>DISP_BLINK_FREQUENCY) runState.disSetTimeBlinkCnt=0;
}

void LcdShowCfgCtrParam_433OnOff(void)
{
    vClearScreen();
    Lcd_Show_Num(r, A1);
    Lcd_Show_Num(LINE, A2);
    Lcd_Set_Segbyid(RF,1);
//    Lcd_Set_Segbyid(DIO1,1);
//    Lcd_Set_Segbyid(DIO2,1);

    switch(GetDisSetTimeState())
    {
    case DISSETTIME_STATE_NULL:
        if(GetDisSet433OnOff())
        {
            Lcd_Show_Num(O, A3);
            Lcd_Show_Num(N, A4);
        }else
        {
            Lcd_Show_Num(O, A3);
            Lcd_Show_Num(F, A4);
        }
        break;
    case DISSETTIME_STATE_MINUTE:
        if(runState.disSetTimeBlinkCnt<=(DISP_BLINK_FREQUENCY/2))
        {
            if(GetDisSet433OnOff())
            {
                Lcd_Show_Num(O, A3);
                Lcd_Show_Num(N, A4);
            }else
            {
                Lcd_Show_Num(O, A3);
                Lcd_Show_Num(F, A4);
            }
        }else if(runState.disSetTimeBlinkCnt>(DISP_BLINK_FREQUENCY/2))
        {
            Lcd_Show_Num(NODISP, A3);
            Lcd_Show_Num(NODISP, A4);
        }
        runState.disSetTimeBlinkCnt ++;
        break;
    }
    if(runState.disSetTimeBlinkCnt>DISP_BLINK_FREQUENCY) runState.disSetTimeBlinkCnt=0;
}

void LcdShowCfgCtrParam_Temper_StandbySW(void)
{
    vClearScreen();
    Lcd_Show_Num(A, A1);
    Lcd_Show_Num(U, A2);
//    Lcd_Set_Segbyid(DIO1,1);
//    Lcd_Set_Segbyid(DIO2,1);

    switch(GetDisSetTimeState())
    {
    case DISSETTIME_STATE_NULL:
        if(GetDisSetTemperModeStandby())
        {
            Lcd_Show_Num(O, A3);
            Lcd_Show_Num(N, A4);
        }else
        {
            Lcd_Show_Num(O, A3);
            Lcd_Show_Num(F, A4);
        }
        break;
    case DISSETTIME_STATE_MINUTE:
        if(runState.disSetTimeBlinkCnt<=(DISP_BLINK_FREQUENCY/2))
        {
            if(GetDisSetTemperModeStandby())
            {
                Lcd_Show_Num(O, A3);
                Lcd_Show_Num(N, A4);
            }else
            {
                Lcd_Show_Num(O, A3);
                Lcd_Show_Num(F, A4);
            }
        }else if(runState.disSetTimeBlinkCnt>(DISP_BLINK_FREQUENCY/2))
        {
            Lcd_Show_Num(NODISP, A3);
            Lcd_Show_Num(NODISP, A4);
        }
        runState.disSetTimeBlinkCnt ++;
        break;
    }
    if(runState.disSetTimeBlinkCnt>DISP_BLINK_FREQUENCY) runState.disSetTimeBlinkCnt=0;
}

//摄氏华氏切换
void LcdShowCfgCtrParam_CtoF(void)
{
    vClearScreen();
//    Lcd_Show_Num(F, A1);
//    Lcd_Show_Num(9, A2);
//    Lcd_Set_Segbyid(DIO1,1);
//    Lcd_Set_Segbyid(DIO2,1);

    switch(GetDisSetTimeState())
    {
    case DISSETTIME_STATE_NULL:
        if(GetConfigData_tempUnit()==TEMPER_UINT_C)
        {
//            Lcd_Show_Num(cc,A4);
            Lcd_Set_Segbyid(TC,1);
            Lcd_Show_Num(1, A2);
            Lcd_Show_Num(POINT, A3);
            Lcd_Show_Num(0, A4);
        }else
        {
            Lcd_Set_Segbyid(TF,1);
            Lcd_Show_Num(3, A1);
            Lcd_Show_Num(3, A2);
            Lcd_Show_Num(POINT, A3);
            Lcd_Show_Num(8, A4);
        }
        break;
    case DISSETTIME_STATE_MINUTE:
        if(GetConfigData_tempUnit()==TEMPER_UINT_C)
        {
            if ((RTC_GetCycle32k() / 17000) % 2)    Lcd_Set_Segbyid(TC,1);
            else                                    Lcd_Set_Segbyid(TC,0);
            Lcd_Show_Num(1, A2);
            Lcd_Show_Num(POINT, A3);
            Lcd_Show_Num(0, A4);
        }else
        {
            if ((RTC_GetCycle32k() / 17000) % 2)    Lcd_Set_Segbyid(TF,1);
            else                                    Lcd_Set_Segbyid(TF,0);
            Lcd_Show_Num(3, A1);
            Lcd_Show_Num(3, A2);
            Lcd_Show_Num(POINT, A3);
            Lcd_Show_Num(8, A4);
        }
        break;
    }
}

//背光时间控制
void LcdShowCfgCtrParam_Bled_Ctl(void)
{
    vClearScreen();
    Lcd_Show_Num(L, A1);
    Lcd_Show_Num(LINE, A2);
    switch(GetDisSetTimeState())
    {
    case DISSETTIME_STATE_NULL:
        Lcd_Show_Num(GetConfigData_BledDuty()/10, A3);
        Lcd_Show_Num(GetConfigData_BledDuty()%10, A4);
        runState.bledDuty = CMD_DISCTRL;
        break;
    case DISSETTIME_STATE_MINUTE:
        if(runState.disSetTimeBlinkCnt<=(DISP_BLINK_FREQUENCY/2))
        {
            Lcd_Show_Num(GetConfigData_BledDuty()/10, A3);
            Lcd_Show_Num(GetConfigData_BledDuty()%10, A4);
        }else if(runState.disSetTimeBlinkCnt>(DISP_BLINK_FREQUENCY/2))
        {
            Lcd_Show_Num(NODISP, A3);
            Lcd_Show_Num(NODISP, A4);
        }
        runState.disSetTimeBlinkCnt ++;
        switch(GetConfigData_BledDuty())
            {
            case 0:runState.bledDuty = 0x80;
                break;
            case 1:
                runState.bledDuty = 0x88;
                break;
            case 2:
                runState.bledDuty = 0x89;
                break;
            case 3:
                runState.bledDuty = 0x8A;
                break;
            case 4:
                runState.bledDuty = 0x8B;
                break;
            case 5:
                runState.bledDuty = 0x8C;
                break;
            case 6:
                runState.bledDuty = 0x8D;
                break;
            case 7:
                runState.bledDuty = 0x8E;
                break;
            case 8:
                runState.bledDuty = CMD_DISCTRL;
                break;
            }
        break;
    }
    if(runState.disSetTimeBlinkCnt>DISP_BLINK_FREQUENCY) runState.disSetTimeBlinkCnt=0;
}

void LcdShowCfg_433CO_Off(void)
{
    vClearScreen();

    Lcd_Set_Segbyid(RF, 1);
    Lcd_Show_Num(C, A1);
    Lcd_Show_Num(LINE, A2);

    switch(GetDisSetTimeState())
    {
    case DISSETTIME_STATE_NULL:
        if(baseParameters.co433Off==TRUE)
        {
            Lcd_Show_Num(O, A3);
            Lcd_Show_Num(N, A4);
        }else
        {
            Lcd_Show_Num(O, A3);
            Lcd_Show_Num(F, A4);
        }
        break;
    case DISSETTIME_STATE_MINUTE:
        if(runState.disSetTimeBlinkCnt<=(DISP_BLINK_FREQUENCY/2))
        {
            if(baseParameters.co433Off==TRUE)
            {
                Lcd_Show_Num(O, A3);
                Lcd_Show_Num(N, A4);
            }else
            {
                Lcd_Show_Num(O, A3);
                Lcd_Show_Num(F, A4);
            }
        }else if(runState.disSetTimeBlinkCnt>(DISP_BLINK_FREQUENCY/2))
        {
            Lcd_Show_Num(NODISP, A3);
            Lcd_Show_Num(NODISP, A4);
        }
        runState.disSetTimeBlinkCnt ++;
        break;
    }
    if(runState.disSetTimeBlinkCnt>DISP_BLINK_FREQUENCY) runState.disSetTimeBlinkCnt=0;
}

void LcdShowCfg_433CO_WarningNum(void)
{
    vClearScreen();

    Lcd_Set_Segbyid(WR, 1);
//    Lcd_Set_Segbyid(ER, 1);

    switch(GetDisSetTimeState())
    {
    case DISSETTIME_STATE_NULL:
        Lcd_Show_Num(baseParameters.coWarningData/100, A2);
        Lcd_Show_Num((baseParameters.coWarningData%100)/10, A3);
        Lcd_Show_Num(baseParameters.coWarningData%10, A4);
        break;
    case DISSETTIME_STATE_MINUTE:
        if(runState.disSetTimeBlinkCnt<=(DISP_BLINK_FREQUENCY/2))
        {
            Lcd_Show_Num(baseParameters.coWarningData/100, A2);
            Lcd_Show_Num((baseParameters.coWarningData%100)/10, A3);
            Lcd_Show_Num(baseParameters.coWarningData%10, A4);
        }
        runState.disSetTimeBlinkCnt ++;
        break;
    }
    if(runState.disSetTimeBlinkCnt>DISP_BLINK_FREQUENCY) runState.disSetTimeBlinkCnt=0;
}

//void LcdShowCfgCtrParam_OilCasl_Full(void)
//{
//    uint32 oillevel=0;
//    vClearScreen();
//    Lcd_Show_Num(F, A1);
//    Lcd_Show_Num(B, A2);
//}

//手动泵油
void Lcd_Show_Manupump(void)
{
    static uint16_t cnt=MANUPUMP_TIMESPACE;
//    static uint8_t zhiyou_cnt=200;

    vClearScreen();
    Lcd_Bk_On();
    if(heaterDatas.runningMode == HEATER_RUNMODE_PUMPOIL)
    {
        Lcd_Show_Num(cnt/1000, A1);
        Lcd_Show_Num((cnt%1000)/100, A2);
        Lcd_Show_Num((cnt%100)/10, A3);
        cnt --;
        if(cnt==0){
            cnt=MANUPUMP_TIMESPACE;
            runState.firstPowerOn = 1;
            TurnOff_Device();
        }
    }else if(heaterDatas.runningMode==HEATER_RUNMODE_NULL || heaterDatas.runningMode==HEATER_RUNMODE_STANDBY || heaterDatas.runningMode==HEATER_RUNMODE_AUTO)
    {
        Lcd_Show_Num(cnt/1000, A1);
        Lcd_Show_Num((cnt%1000)/100, A2);
        Lcd_Show_Num((cnt%100)/10, A3);
        if(cnt<MANUPUMP_TIMESPACE)
        {
            cnt--;
        }
        if(cnt==0){
            cnt=MANUPUMP_TIMESPACE;
            runState.firstPowerOn = 1;
            TurnOff_Device();
        }
    }else {
        cnt=MANUPUMP_TIMESPACE;
        runState.pumPoilFlag = 0;
        Lcd_Show_Num(3, A1);
        Lcd_Show_Num(0, A2);
        Lcd_Show_Num(0, A3);
    }
    //如果是至优主板进入泵油后直接跳转运行界面
//    if(runState.mainBoardType == MAIN_BOARD_TYPE_VENTILATE_FASTHART)
//    {
//        zhiyou_cnt --;
//        if(zhiyou_cnt==0 && runState.pumPoilFlag == 1)
//        {
//            //直接跳转运行界面
//            runState.ctrlCmdFlag = 1;
//            runState.disIndex = DISINDEX_RUNNING;    //开机动画显示
//            runState.firstPowerOn = 1;
//            runState.offingBlinkTime = OFFING_BLINK_TIME;
//            runState.temperStandbyState = TEMPER_STADNBY_STATE_NULL;
//            runState.standbyOnOffDisState = TEMPER_STADNBY_STATE_NULL;
//            runState.bleSetPassword = 0;
//            runState.runModel = REMOTE_STATE_POWER_ON;
//        }
//    }
    if(ykq_key==rf_code[3] && ykq_key!=0 && ykq_key!=0xFFFFFFFF) //遥控器电源
    {
        vEV3P035JPlay(BE_VOICE);
        runState.firstPowerOn = 1;
        TurnOff_Device();
        return;
    }
}
