/*
 * button.c
 *
 *  Created on: 2023年6月20日
 *      Author: wx030
 */

/******************************************************************************/
/* 头文件包含 */
#include "HAL.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "multi_button.h"
#include "peripheral.h"

/**************************************************************************************************
 *                                        GLOBAL VARIABLES
 **************************************************************************************************/
uint8_t btnPower_id = 0;
uint8_t btnMenu_id = 1;
uint8_t btnAdd_id = 2;
uint8_t btnDec_id = 3;
uint8_t btnEng_id = 4;//+ -同时按进入工程模式
struct Button btnPower,btnMenu,btnAdd,btnDec,btnEng;


uint8_t read_button_GPIO(uint8_t button_id)
{
    // A2610 按键映射 (CH584)
    // 加键: PB9, 减键: PB8, 菜单键: PB17, 电源键: PB16
    switch (button_id)
    {
    case 0:  // 电源键 PB16
        return ((GPIOB_ReadPortPin(GPIO_Pin_16) == 0) ? 0 : 1);
        break;
    case 1:  // 菜单键 PB17
        return ((GPIOB_ReadPortPin(GPIO_Pin_17) == 0) ? 0 : 1);
        break;
    case 2:  // 加键 PB9
        return ((GPIOB_ReadPortPin(GPIO_Pin_9) == 0) ? 0 : 1);
        break;
    case 3:  // 减键 PB8
        return ((GPIOB_ReadPortPin(GPIO_Pin_8) == 0) ? 0 : 1);
        break;
    case 4:  // 工程模式 (+ - 同时按)
        return ((GPIOB_ReadPortPin(GPIO_Pin_9) == 0) && (GPIOB_ReadPortPin(GPIO_Pin_8) == 0) ? 0 : 1);
        break;
    default:
        return 0;
    }
}

static u8 set=0;//开机自检显示、清除遥控使用变量
void button_polling(void)
{
    static uint8_t cnt=0,off_cnt=0;
    if(runState.coOverFlag==1)
    {
        if(cnt<=50)
        {
            cnt++;
        }
        if(cnt>50)
        {
#if CO_VALUE==0
            runState.disIndex = DISINDEX_CO_OVERWARNING;
#elif CO_VALUE==1
            //跳转CO超标页面
            if(runState.disIndex!=DISINDEX_CO_VALUE)
                runState.disIndex = DISINDEX_CO_OVERWARNING;
#endif
        }
    }
    else
    {
        cnt = 0;
    }
    if(ykq_key==CO_OFF_CODE && baseParameters.co433Off==TRUE)
    {
        runState.coTurnOffFlag = 1;
        runState.coBeepFlag = 1;
    }
    if(runState.coTurnOffFlag==1 && runState.runModel != REMOTE_STATE_POWER_OFF)
    {
        //关机
        runState.runModel = REMOTE_STATE_POWER_OFF;
        TurnOff_Device();
        //显示E11
        runState.disIndex = DISINDEX_CO_OVERWARNING;
    }
    switch(runState.disIndex)
    {
    case DISINDEX_NULL://上电自检
        if (set <= HT1628A_BYTES * 2)
        {
            Lcd_Set_Segbyid(3 * set, 1);
            Lcd_Set_Segbyid(3 * set+1, 1);
            Lcd_Set_Segbyid(3 * set+2, 1);
        }
        else if (set > HT1628A_BYTES * 2 && set <= (HT1628A_BYTES*2)+2)
        {
            vClearScreen();
        }
        if(set>(HT1628A_BYTES*2)+2)
        {
            set = 0;
            runState.disIndex = DISINDEX_VERSION;
        }
        set++;
        break;
    case DISINDEX_VERSION:
        Lcd_Show_Num(u, A1);
        Lcd_Show_Num((VERSION % 1000) / 100, A2);
        Lcd_Show_Num((VERSION % 100) / 10, A3);
        Lcd_Show_Num(VERSION % 10, A4);
        if (set > 10)
        {
            vClearScreen();
            Lcd_Bk_Off();
            runState.disIndex = DISINDEX_PWROFFED;    //关机
            set = 0;
        }
        set++;
        break;
    case DISINDEX_REMOTECLEAR://清除遥控器
        if(set<=30)
        {
            Lcd_Show_Num(C, A1);
            Lcd_Show_Num(L, A2);
            Lcd_Show_Num(r, A3);
            Lcd_Show_Num(NODISP, A4);
            set++;
        }
        else if(set>30 && set<=60)
        {
            Lcd_Show_Num(S, A1);
            Lcd_Show_Num(U, A2);
            Lcd_Show_Num(C, A3);
            Lcd_Show_Num(NODISP, A4);
            if(set==31)
            {
                rf_code[0] = 0xFF;
                rf_code[1] = 0xFF;
                rf_code[2] = 0xFF;
                rf_code[3] = 0xFF;

                baseParameters.remoteEn = FALSE;
                vPARA_SAVE();
            }
            set++;
        }
        else if(set>60)
        {
            vClearScreen();
            Lcd_Bk_Off();
            runState.disIndex = DISINDEX_PWROFFED;    //关机
            set = 0;
        }
        break;
    case DISINDEX_AUTOTMP_REMOTETEMP:
        if(set<=30)
        {
            Lcd_Show_Num(NODISP, A1);
            Lcd_Show_Num(C, A2);
            Lcd_Show_Num(T, A3);
            Lcd_Show_Num(NODISP, A4);
            set++;
        }
        else if(set>30 && set<=60)
        {
            if(set==31)
            {
                if(baseParameters.autoTempRemoteTempEn) baseParameters.autoTempRemoteTempEn = FALSE;
                else                                    baseParameters.autoTempRemoteTempEn = TRUE;
                vPARA_SAVE();
            }
            Lcd_Show_Num(baseParameters.autoTempRemoteTempEn, A4);
            set++;
        }
        else if(set>60)
        {
            vClearScreen();
            Lcd_Bk_Off();
            runState.disIndex = DISINDEX_PWROFFED;    //关机
            set = 0;
        }
        break;
    case DISINDEX_PWROFFED:
        vClearScreen();
        runState.runModel = REMOTE_STATE_POWER_OFF;

        //清除串口数据
        clear_State();
//        if(ykq_key==rf_code[3] && ykq_key!=0 && ykq_key!=0xFFFFFFFF) //遥控器电源   需要学习4键
        Lcd_Show_Time(gettimebyoffset(),GetDisSetTimerClockEn());
        Lcd_Bk_Off();
        if(ykq_key==rf_code[2] && ykq_key!=0 && ykq_key!=0xFFFFFFFF) //遥控器电源  学习单键
        {
            TurnOn_Device();
        }
        break;
    case DISINDEX_PWRON_ANIMATION:
        Lcd_Bk_On();
        runState.runModel = REMOTE_STATE_POWER_ON;
        if(Lcd_Show_PwrOnAnimation())
        {
            runState.disIndex = DISINDEX_RUNNING;
            heaterDatas.runningStep = HEATERRUNSTEP_NULL;
            //播放开始加热
            vEV3P035JPlay(START_HEAT);
            UART1_CMDON();  //发送开机给主机
            runState.bklight_time = GetConfigData_BledTime()*10;
        }
        break;
    case DISINDEX_PWROFF_ANIMATION:
        Lcd_Bk_On();
        if(Lcd_Show_PwrOffAnimation())
        {
            runState.offingBlinkTime = OFFING_BLINK_TIME;
            runState.disIndex = DISINDEX_OFFING;
            runState.runModel = REMOTE_STATE_POWER_OFF;
            //播放停止加热
            vEV3P035JPlay(STOP_HEART);
            UART1_CMDOFF(); //发送关机给主机
        }
        break;
    case DISINDEX_RUNNING://
        Lcd_ShowRunning();
        break;
    case DISINDEX_RUNNING_FAULT://
        Lcd_ShowRunning_Fault();
        break;
#if CO_VALUE==1
    case DISINDEX_CO_VALUE:
        Lcd_ShowCO_Value();
        break;
#endif
    case DISINDEX_CO_OVERWARNING:
        Lcd_ShowCO_OverWarning();
        break;
    case DISINDEX_OFFING://等待关机完成
        if(Lcd_Show_PwrOffing())
        {
            clear_State();
            //播放谢谢使用
            vEV3P035JPlay(THANKS);
        }
        break;
    case DISINDEX_ENGMODE_HEATERFW: //工程模式-主机版本号
        Lcd_ShowEngMode_HeaterFW();
        break;
    case DISINDEX_ENGMODE_FAULTTYPE:
        Lcd_ShowEngMode_FaultType();
        break;
    case DISINDEX_ENGMODE_SHELLTEMPER:
        Lcd_ShowEngMode_ShellTemper();
        break;
    case DISINDEX_ENGMODE_SUPVOL:
        Lcd_ShowEngMode_SupVol();
        break;
    case DISINDEX_ENGMODE_RUNLEVEL:
        Lcd_ShowEngMode_RunLevel();
        break;
    case DISINDEX_ENGMODE_ROOMTEMP:
        Lcd_ShowEngMode_RoomTemp();
        break;
    case DISINDEX_ENGMODE_ALTITUDE:
        Lcd_ShowEngMode_Altitude();
        break;
    case DISINDEX_ENGMODE_MANUPUMP:
        Lcd_ShowEngMode_ManUpUmp();
        break;
    case DISINDEX_ENGMODE_REMOTEMATCH://添加遥控
        Lcd_ShowEngMode_RemoteMatch();
        break;
    case DISINDEX_ENGMODE_BLEMATCH://蓝牙匹配
        Lcd_ShowEngMode_BleMatch();
        break;
    case DISINDEX_ENGMODE_REST:
        Lcd_ShowEngMode_Rest();
        break;
#if VCC_CHICK==1
    case DISINDEX_ENGMODE_VCC://电源纹波检测显示
        Lcd_ShowEngMode_Vcc();
        break;
#endif
    case DISINDEX_CFGCTRLPARAM_RTC: //基本参数设置-设备时间
        LcdShowCfgCtrParam_RTC();
        break;
    case DISINDEX_CFGCTRLPARAM_TIMER: //基本参数设置-定时开机时间
        LcdShowCfgCtrParam_Timer();
        break;
    case DISINDEX_CFGCTRLPARAM_TIMER_RUNTIME: //基本参数设置-定时开机运行时间
        LcdShowCfgCtrParam_Timer_RunTime();
        break;
    case DISINDEX_CFGCTRLPARAM_TIMER_SW: //基本参数设置-定时开机使能
        LcdShowCfgCtrParam_Timer_Sw();
        break;
    case DISINDEX_CFGCTRLPARAM_VOICE_SW: //基本参数设置-语音功能使能
        LcdShowCfgCtrParam_Voice_Sw();
        break;
    case DISINDEX_CFGCTRLPARAM_TEMPEROFFSET: //基本参数设置-温度传感器偏差设置
        LcdShowCfgCtrParam_TemperOffSet();
        break;
//    case DISINDEX_CFGCTRLPARAM_OILCASL_VOLUME: //基本参数设置-油箱容积选择
//        LcdShowCfgCtrParam_Oilcasl_Volume();
//        break;
//    case DISINDEX_CFGCTRLPARAM_OILPUMP_TYPE: //基本参数设置-油泵类型选择
//        LcdShowCfgCtrParam_Oilpump_Type();
//        break;
    case DISINDEX_CFGCTRLPARAM_BLE_ONOFF://蓝牙开关
        LcdShowCfgCtrParam_BLEOnOff();
        break;
    case DISINDEX_CFGCTRLPARAM_433_ONOFF://433开关
        LcdShowCfgCtrParam_433OnOff();
        break;
    case DISINDEX_CFGCTRLPARAM_TEMPER_STANDBY_SW: //基本参数设置-温度自动控制使能
        LcdShowCfgCtrParam_Temper_StandbySW();
        break;
    case DISINDEX_CFGCTRLPARAM_C_TO_F://温度单位切换
        LcdShowCfgCtrParam_CtoF();
        break;
    case DISINDEX_CFGCTRLPARAM_BLED_CTL://背光时间设置
        LcdShowCfgCtrParam_Bled_Ctl();
        break;
    case DISINDEX_CFG_433CO_OFF://433接收CO超标固定关机码
        LcdShowCfg_433CO_Off();
        break;
#if CO_VALUE==1
    case DISINDEX_CFGCTRLPARAM_CO_WARNING://CO报警值设置
        LcdShowCfg_433CO_WarningNum();
        break;
#endif
//    case DISINDEX_CFGCTRLPARAM_OILCASL_FULL://手动将油箱置满，用于计算剩余油量
//        LcdShowCfgCtrParam_OilCasl_Full();
//        break;
    case DISINDEX_MANUPUMP_STATE://泵油
        Lcd_Show_Manupump();
        break;
    }
    if(runState.usart1DelayCnt>0) runState.usart1DelayCnt--;
    if(runState.usart1DelayCnt==0 && runState.disIndex != DISINDEX_PWROFFED)
    {
        //一直串口发送数据，通过比较遥控器状态和主机状态判断发送控制指令还是发送获取状态指令
        switch(runState.runModel)
        {
        case REMOTE_STATE_POWER_ON:
            if(runState.temperStandbyState == TEMPER_STADNBY_STATE_OFF)
            {
                if(heaterDatas.runningStep != HEATERRUNSTEP_NULL)
                {
                    //关机
                    UART1_CMDOFF();
                }else
                {
                    //获取主机状态
                    //UART1_GETDATA();
                }
            }else
            {
                if(baseParameters.runMode == CTRL_RUNMODE_VENTILATE)
                {
                    if(heaterDatas.runningMode != HEATER_RUNMODE_VENTILATE) UART1_CMDON();
                    else UART1_GETDATA();
                }else
                {
                    if(runState.mainBoardType != MAIN_BOARD_TYPE_VENTILATE_FASTHART)
                    {
                        if(heaterDatas.runningMode == HEATER_RUNMODE_MANU)
                        {
                            UART1_GETDATA();
                        }else {
                            if(heaterDatas.runningMode != HEATER_RUNMODE_AUTO)  UART1_CMDON();
                            else UART1_GETDATA();
                        }
                    }
                    else
                    {
                        if(runState.ctrlCmdFlag == 1)  UART1_CMDON();
                        else UART1_GETDATA();
                    }
                }
            }
            break;
        case REMOTE_STATE_POWER_OFF:
            if(heaterDatas.runningStep != HEATERRUNSTEP_NULL || heaterDatas.runningMode != HEATER_RUNMODE_NULL)
            {
                //关机
                UART1_CMDOFF();
            }
            break;
        case REMOTE_STATE_PUMPOIL:
            if(heaterDatas.runningMode != HEATER_RUNMODE_PUMPOIL)
            {
                if(runState.pumPoilFlag)
                {
                    UART1_GETDATA();
                }
                else {
                    //泵油
                    UART1_CMDPUMPOIL();
                }
            }
            else {
                runState.pumPoilFlag = 1;
                UART1_GETDATA();
            }
            break;
        }
    }
    ykq_key = 0xFFFFFFFF;
    vFreshScreen();
}
/*********************************************************************
 * @fn      TMR0_IRQHandler
 *
 * @brief   TMR0中断函数
 *
 * @return  none
 */
#define REVERSEBIT(x,y) x^=(1<<y)
static void btn_event_callback(void* btn)
{
    Button* temp_button = (Button*) btn;
    static u8 state=0;

    switch(runState.disIndex)
    {
    case DISINDEX_NULL:
        break;
    case DISINDEX_VERSION:
        if (temp_button->button_id == btnPower_id &&
            (get_button_event(temp_button) == LONG_PRESS_HOLD || get_button_event(temp_button) == LONG_PRESS_START))
        {
//          vEV3P035JPlay(BE_VOICE);
            runState.disIndex = DISINDEX_REMOTECLEAR;
        }
        if (temp_button->button_id == btnMenu_id &&
            (get_button_event(temp_button) == LONG_PRESS_HOLD || get_button_event(temp_button) == LONG_PRESS_START))
        {
//            vEV3P035JPlay(BE_VOICE);
            runState.disIndex = DISINDEX_AUTOTMP_REMOTETEMP;
        }
        break;
    case DISINDEX_REMOTECLEAR:
        if ((temp_button->button_id == btnPower_id && get_button_event(temp_button) == PRESS_UP))
        {
            set = 0;
            vClearScreen();
            Lcd_Bk_Off();
            runState.disIndex = DISINDEX_PWROFFED;    //关机
        }
        break;
    case DISINDEX_PWRON_ANIMATION:
//      if(temp_button->button_id == btnPower_id && get_button_event(temp_button) == PRESS_UP)
//        {
//            runState.disIndexBefore = DISINDEX_RUNNING;
//        }
        break;
    case DISINDEX_MANUPUMP_STATE:
        if (temp_button->button_id == btnPower_id &&
                        (get_button_event(temp_button) == SINGLE_CLICK || get_button_event(temp_button) == PRESS_DOWN))
        {
            vEV3P035JPlay(BE_VOICE);
            runState.firstPowerOn = 1;
            TurnOff_Device();
        }
        break;
    case DISINDEX_OFFING:
        if (temp_button->button_id == btnPower_id && get_button_event(temp_button) == SINGLE_CLICK)
        {
            vEV3P035JPlay(BE_VOICE);
            runState.disIndex = DISINDEX_PWROFFED;
        }
        break;
    case DISINDEX_PWROFFED:
        //电源键短按、按下
        if (temp_button->button_id == btnPower_id && get_button_event(temp_button) == SINGLE_CLICK)
        {
            TurnOn_Device();
        }
        //-长按进入泵油模式
        if (temp_button->button_id==btnDec_id && get_button_event(temp_button)==LONG_PRESS_START && runState.firstPowerOn==0){
            runState.disIndex = DISINDEX_MANUPUMP_STATE;
            runState.runModel = REMOTE_STATE_PUMPOIL;
        }
        //菜单键短按
//        if (temp_button->button_id == btnMenu_id && get_button_event(temp_button) == DOUBLE_CLICK){
//            runState.disIndex = DISINDEX_ENGMODE_REMOTEMATCH;   //跳转清除遥控页面，后长按旋钮显示SUC清除遥控码
//            runState.remoteMatchState = DISSETENGMODE_REMOTEMATCH_KEYADD;
//            //打开背光
//            Lcd_Bk_On();
//        }
        break;
//  case DISINDEX_PWRON_ANIMATION:
//
//      break;
//  case DISINDEX_PWROFF_ANIMATION:
//      break;
    case DISINDEX_RUNNING:
        //关机
        if(temp_button->button_id == btnPower_id && get_button_event(temp_button) == SINGLE_CLICK)
        {
            vEV3P035JPlay(BE_VOICE);
            if(GetConfigData_BledDuty()==BLED_DUTY_MAX)
            {
                TurnOff_Device();
            }else {
                if(runState.bklight_time==0)
                {
                    runState.bklight_time = GetConfigData_BledTime()*10;
                }else {
                    TurnOff_Device();
                }
            }
        }

        //菜单短按
        if (temp_button->button_id == btnMenu_id && get_button_event(temp_button) == SINGLE_CLICK){
            vEV3P035JPlay(BE_VOICE);
            runState.bklight_time = GetConfigData_BledTime()*10;
            //控制模式切换
            ControlModel_Change();
        }

        //加减按键操作
        if (temp_button->button_id == btnDec_id && get_button_event(temp_button) == PRESS_DOWN)
        {
            vEV3P035JPlay(BE_VOICE);
            runState.bklight_time = GetConfigData_BledTime()*10;
            //-
            switch(GetConfigData_RunMode())
            {
            case CTRL_RUNMODE_TEMP://温度自控
                CleardisSetTempBlinkCnt();
                SetDisSetTemperBlink(1);
                RunningSetTemper_Dec();
                break;
            case CTRL_RUNMODE_LEVEL://手动
                RunningSetLevel_Dec();
                break;
            case CTRL_RUNMODE_VENTILATE://通风
                RunningVentilateLevel_Dec();
                break;
            }
        }
        if (temp_button->button_id == btnAdd_id && get_button_event(temp_button) == PRESS_DOWN)
        {
            vEV3P035JPlay(BE_VOICE);
            runState.bklight_time = GetConfigData_BledTime()*10;
            //+
            switch(GetConfigData_RunMode())
            {
            case CTRL_RUNMODE_TEMP://温度自控
                CleardisSetTempBlinkCnt();
                SetDisSetTemperBlink(1);
                RunningSetTemper_Add();
                break;
            case CTRL_RUNMODE_LEVEL://手动
                RunningSetLevel_Add();
                break;
            case CTRL_RUNMODE_VENTILATE://通风
                RunningVentilateLevel_Add();
                break;
            }
        }
        //+&-长按5s
        if (temp_button->button_id == btnEng_id && get_button_event(temp_button) == LONG_PRESS_START){
            vEV3P035JPlay(BE_VOICE);
            //进入工程模式,显示主机版本
            vClearScreen();
            runState.bklight_time = GetConfigData_BledTime()*10;
            runState.disIndex = DISINDEX_ENGMODE_HEATERFW;
        }
        //+长按5s 添加遥控
        if ((temp_button->button_id == btnAdd_id && get_button_event(temp_button) == LONG_PRESS_START) && (GPIOA_ReadPortPin(GPIO_Pin_5) != 0))
        {
            vEV3P035JPlay(BE_VOICE);
            //进入添加遥控
            runState.bklight_time = GetConfigData_BledTime()*10;
            runState.disIndex = DISINDEX_ENGMODE_REMOTEMATCH;
            runState.remoteMatchState = DISSETENGMODE_REMOTEMATCH_KEYADD;
        }
        //-长按5s 语音切换
        if ((temp_button->button_id == btnDec_id && get_button_event(temp_button) == LONG_PRESS_START) && (GPIOA_ReadPortPin(GPIO_Pin_4) != 0))
        {
            vEV3P035JPlay(BE_VOICE);
            SetDisSetLanguageTypeAdd();
            //进入语音切换
            runState.bklight_time = GetConfigData_BledTime()*10;
            runState.disIndex = DISINDEX_CFGCTRLPARAM_VOICE_SW;
        }
        //菜单长按5s->参数设置
        if ((temp_button->button_id == btnMenu_id && get_button_event(temp_button) == LONG_PRESS_START) && (GPIOA_ReadPortPin(GPIO_Pin_5) != 0))
        {
            vEV3P035JPlay(BE_VOICE);
            //进入基本参数设置模式,设置设备时间
            runState.bklight_time = GetConfigData_BledTime()*10;
            runState.disIndex = DISINDEX_CFGCTRLPARAM_RTC;
        }
        //关机长按5s co数值显示
#if CO_VALUE==1
        if (temp_button->button_id == btnPower_id && get_button_event(temp_button) == LONG_PRESS_START){
            vEV3P035JPlay(BE_VOICE);
            runState.bklight_time = GetConfigData_BledTime()*10;
            runState.disIndex = DISINDEX_CO_VALUE;
        }
#endif
        break;
    case DISINDEX_RUNNING_FAULT:
        if ((temp_button->button_id == btnPower_id && get_button_event(temp_button) == PRESS_DOWN)){
            vEV3P035JPlay(BE_VOICE);
            if(GetConfigData_BledTime()==0)
            {
                TurnOff_Device();
            }else {
                if(runState.bklight_time==0)
                {
                    if(runState.voiceIntervalCnt == VOICE_INTERVAL_CNT) runState.voiceIntervalCnt = 0;
                    runState.bklight_time = GetConfigData_BledTime()*10;
                }else {
                    TurnOff_Device();
                }
            }
        }
        if ((temp_button->button_id == btnMenu_id && get_button_event(temp_button) == PRESS_DOWN))
        {
            vEV3P035JPlay(BE_VOICE);
            if(runState.voiceIntervalCnt == VOICE_INTERVAL_CNT) runState.voiceIntervalCnt = 0;
            //打开背光
            runState.bklight_time = GetConfigData_BledTime()*10;
        }
        if ((temp_button->button_id == btnAdd_id && get_button_event(temp_button) == PRESS_DOWN))
        {
            vEV3P035JPlay(BE_VOICE);
            if(runState.voiceIntervalCnt == VOICE_INTERVAL_CNT) runState.voiceIntervalCnt = 0;
            //打开背光
            runState.bklight_time = GetConfigData_BledTime()*10;
        }
        if ((temp_button->button_id == btnDec_id && get_button_event(temp_button) == PRESS_DOWN))
        {
            vEV3P035JPlay(BE_VOICE);
            if(runState.voiceIntervalCnt == VOICE_INTERVAL_CNT) runState.voiceIntervalCnt = 0;
            //打开背光
            runState.bklight_time = GetConfigData_BledTime()*10;
        }
        break;
#if CO_VALUE==1
    case DISINDEX_CO_VALUE:
        //关机短按
        if (temp_button->button_id == btnPower_id && get_button_event(temp_button) == SINGLE_CLICK){
            vEV3P035JPlay(BE_VOICE);
            vClearScreen();
            runState.disIndex = DISINDEX_RUNNING;
            runState.bklight_time = GetConfigData_BledTime()*10;
        }
        break;
#endif
    case DISINDEX_CO_OVERWARNING:
        if (temp_button->button_id == btnPower_id && get_button_event(temp_button) == SINGLE_CLICK){
            vEV3P035JPlay(BE_VOICE);
            TurnOff_Device();
        }
        if ((temp_button->button_id == btnAdd_id||temp_button->button_id == btnDec_id)
                        && get_button_event(temp_button) == SINGLE_CLICK)
        {
            runState.coBeepFlag = 0;
            if(runState.coDisTime==0)   runState.coDisTime = 100;
            else                        runState.coDisTime = 0;
        }
        break;
    case DISINDEX_ENGMODE_HEATERFW:
    case DISINDEX_ENGMODE_FAULTTYPE:
    case DISINDEX_ENGMODE_SHELLTEMPER:
    case DISINDEX_ENGMODE_SUPVOL:
    case DISINDEX_ENGMODE_RUNLEVEL:
    case DISINDEX_ENGMODE_ROOMTEMP:
    case DISINDEX_ENGMODE_ALTITUDE:
    case DISINDEX_ENGMODE_MANUPUMP:
        if (temp_button->button_id == btnPower_id && get_button_event(temp_button) == SINGLE_CLICK){
            vEV3P035JPlay(BE_VOICE);
            runState.disIndex = DISINDEX_RUNNING;
            runState.bklight_time = GetConfigData_BledTime()*10;
        }
        //不在关机添加遥控界面
        if(runState.runModel != REMOTE_STATE_POWER_OFF)
        {
            //加减按键操作
            if (temp_button->button_id == btnDec_id && get_button_event(temp_button) == PRESS_DOWN)
            {
                vEV3P035JPlay(BE_VOICE);
                SetDisEngModeDec();
            }
            if (temp_button->button_id == btnAdd_id && get_button_event(temp_button) == PRESS_DOWN)
            {
                vEV3P035JPlay(BE_VOICE);
                SetDisEngModeAdd();
            }
        }
        break;
    case DISINDEX_ENGMODE_REMOTEMATCH://添加遥控
        if(runState.runModel != REMOTE_STATE_POWER_OFF && runState.remoteMatchState == DISSETENGMODE_REMOTEMATCH_NULL)
        {
            //加减按键操作
            if (temp_button->button_id == btnDec_id && get_button_event(temp_button) == PRESS_DOWN)
            {
                vEV3P035JPlay(BE_VOICE);
                SetDisEngModeDec();
            }
            if (temp_button->button_id == btnAdd_id && get_button_event(temp_button) == PRESS_DOWN)
            {
                vEV3P035JPlay(BE_VOICE);
                SetDisEngModeAdd();
            }
        }
        if (temp_button->button_id == btnPower_id && get_button_event(temp_button) == SINGLE_CLICK){
            vEV3P035JPlay(BE_VOICE);
            if(runState.runModel != REMOTE_STATE_POWER_OFF)
            {
                vClearScreen();
                runState.disIndex = DISINDEX_RUNNING;
                runState.bklight_time = GetConfigData_BledTime()*10;
                ClearDisSetRemoteMatchState();
            }else{
                //直接关机
                TurnOff_DeviceNow();
            }
        }
//      //旋钮长按5s,进入遥控器添加界面
        if (temp_button->button_id == btnMenu_id && get_button_event(temp_button) == LONG_PRESS_START){
            vEV3P035JPlay(BE_VOICE);
            if(DISSETENGMODE_REMOTEMATCH_NULL == runState.remoteMatchState)
            {
                runState.remoteMatchState = DISSETENGMODE_REMOTEMATCH_KEYADD;
            }
        }
        break;
    case DISINDEX_ENGMODE_BLEMATCH://蓝牙匹配
        if(runState.runModel != REMOTE_STATE_POWER_OFF)
        {
            //加减按键操作
            if (temp_button->button_id == btnDec_id && get_button_event(temp_button) == PRESS_DOWN && runState.disSetTimeState == DISSETTIME_STATE_NULL )
            {
                vEV3P035JPlay(BE_VOICE);
                SetDisEngModeDec();
            }
            if (temp_button->button_id == btnAdd_id && get_button_event(temp_button) == PRESS_DOWN && runState.disSetTimeState == DISSETTIME_STATE_NULL)
            {
                vEV3P035JPlay(BE_VOICE);
                SetDisEngModeAdd();
            }
            //长按进入蓝牙匹配
            if (temp_button->button_id == btnMenu_id && get_button_event(temp_button) == LONG_PRESS_START){
                vEV3P035JPlay(BE_VOICE);
                runState.disSetTimeState = DISSETTIME_STATE_HOUR_MINUTE;
                runState.bleSetPassword = 1;
            }
            //短按退出蓝牙匹配模式
            if (temp_button->button_id == btnMenu_id && get_button_event(temp_button) == SINGLE_CLICK){
                vEV3P035JPlay(BE_VOICE);
                runState.disSetTimeState = DISSETTIME_STATE_NULL;
                runState.bleSetPassword = 0;
            }
        }
        if (temp_button->button_id == btnPower_id && get_button_event(temp_button) == SINGLE_CLICK){
            vEV3P035JPlay(BE_VOICE);
            runState.disIndex = DISINDEX_RUNNING;
            runState.disSetTimeState = DISSETTIME_STATE_NULL;
            runState.bleSetPassword = 0;
            runState.bklight_time = GetConfigData_BledTime()*10;
        }
        break;
    case DISINDEX_ENGMODE_REST:
        //加减按键操作
        if (temp_button->button_id == btnDec_id && get_button_event(temp_button) == PRESS_DOWN && runState.disSetTimeState == DISSETTIME_STATE_NULL )
        {
            vEV3P035JPlay(BE_VOICE);
            SetDisEngModeDec();
        }
        if (temp_button->button_id == btnAdd_id && get_button_event(temp_button) == PRESS_DOWN && runState.disSetTimeState == DISSETTIME_STATE_NULL)
        {
            vEV3P035JPlay(BE_VOICE);
            SetDisEngModeAdd();
        }
        if (temp_button->button_id == btnMenu_id && get_button_event(temp_button) == LONG_PRESS_START)
        {
            vEV3P035JPlay(BE_VOICE);
            //参数恢复默认并重启
            EEPROM_ERASE(0, 256);
            SYS_ResetExecute();
        }
        if (temp_button->button_id == btnPower_id && get_button_event(temp_button) == SINGLE_CLICK){
            vEV3P035JPlay(BE_VOICE);
            runState.disIndex = DISINDEX_RUNNING;
            runState.disSetTimeState = DISSETTIME_STATE_NULL;
            runState.bleSetPassword = 0;
            runState.bklight_time = GetConfigData_BledTime()*10;
        }
        break;
#if VCC_CHICK==1
    case DISINDEX_ENGMODE_VCC://电源纹波检测
        //加减按键操作
        if (temp_button->button_id == btnDec_id && get_button_event(temp_button) == PRESS_DOWN && runState.disSetTimeState == DISSETTIME_STATE_NULL )
        {
            vEV3P035JPlay(BE_VOICE);
            SetDisEngModeDec();
        }
        if (temp_button->button_id == btnAdd_id && get_button_event(temp_button) == PRESS_DOWN && runState.disSetTimeState == DISSETTIME_STATE_NULL)
        {
            vEV3P035JPlay(BE_VOICE);
            SetDisEngModeAdd();
        }
        //短按切换显示
        if (temp_button->button_id == btnMenu_id && get_button_event(temp_button) == SINGLE_CLICK){
            vEV3P035JPlay(BE_VOICE);
            runState.disSetTimeState++;
            if(runState.disSetTimeState > DISSETTIME_STATE_MINUTE)  runState.disSetTimeState = DISSETTIME_STATE_NULL;
        }
        if (temp_button->button_id == btnPower_id && get_button_event(temp_button) == SINGLE_CLICK){
            vEV3P035JPlay(BE_VOICE);
            runState.disIndex = DISINDEX_RUNNING;
            runState.disSetTimeState = DISSETTIME_STATE_NULL;
            runState.bleSetPassword = 0;
            runState.bklight_time = GetConfigData_BledTime()*10;
        }
        break;
#endif
    case DISINDEX_CFGCTRLPARAM_VOICE_SW://语音选择
//        if (temp_button->button_id == btnPower_id && get_button_event(temp_button) == SINGLE_CLICK){
//            vEV3P035JPlay(BE_VOICE);
//            vPARA_SAVE();
//            runState.disSetTimeState = DISSETTIME_STATE_NULL;
//            ClearDisSetTimeBlinkCnt();
//            runState.disIndex = DISINDEX_RUNNING;
//            runState.bklight_time = GetConfigData_BledTime()*10;
//        }
//        if (temp_button->button_id == btnMenu_id && get_button_event(temp_button) == SINGLE_CLICK){
//            vEV3P035JPlay(BE_VOICE);
//            runState.disSetTimeState = (runState.disSetTimeState==DISSETTIME_STATE_NULL) ? DISSETTIME_STATE_MINUTE:DISSETTIME_STATE_NULL;
//        }
//        //加减按键操作
//        if (temp_button->button_id == btnDec_id && get_button_event(temp_button) == PRESS_DOWN)
//        {
//            vEV3P035JPlay(BE_VOICE);
//            switch(GetDisSetTimeState())
//            {
//            case DISSETTIME_STATE_NULL:
//                SetDisRunState_disIndexDec();
//                break;
//            case DISSETTIME_STATE_MINUTE:
//                ClearDisSetTimeBlinkCnt();
//                SetDisSetLanguageTypeDec();
//                break;
//            }
//        }
//        if (temp_button->button_id == btnAdd_id && get_button_event(temp_button) == PRESS_DOWN)
//        {
//            vEV3P035JPlay(BE_VOICE);
//            switch(GetDisSetTimeState())
//            {
//            case DISSETTIME_STATE_NULL:
//                SetDisRunState_disIndexAdd();
//                break;
//            case DISSETTIME_STATE_MINUTE:
//                ClearDisSetTimeBlinkCnt();
//                SetDisSetLanguageTypeAdd();
//                break;
//            }
//        }
//        break;
    case DISINDEX_CFGCTRLPARAM_RTC://基本参数时间设置
    case DISINDEX_CFGCTRLPARAM_TIMER://基本参数定时开机时间
    case DISINDEX_CFGCTRLPARAM_TIMER_RUNTIME://基本参数定时开机运行时间
    case DISINDEX_CFGCTRLPARAM_TIMER_SW://基本参数定时开机运行时间
    case DISINDEX_CFGCTRLPARAM_TEMPEROFFSET://温度偏差设置
//    case DISINDEX_CFGCTRLPARAM_OILCASL_VOLUME://油箱容积选择
//    case DISINDEX_CFGCTRLPARAM_OILPUMP_TYPE://油泵类型选择
    case DISINDEX_CFGCTRLPARAM_BLE_ONOFF://蓝牙开关
    case DISINDEX_CFGCTRLPARAM_433_ONOFF://蓝牙开关
    case DISINDEX_CFGCTRLPARAM_TEMPER_STANDBY_SW://温度自动控制使能
    case DISINDEX_CFGCTRLPARAM_C_TO_F://温度单位切换
    case DISINDEX_CFGCTRLPARAM_BLED_CTL:
    case DISINDEX_CFG_433CO_OFF://433接收CO超标固定关机码
#if CO_VALUE==1
    case DISINDEX_CFGCTRLPARAM_CO_WARNING:
#endif
        if (temp_button->button_id == btnPower_id && get_button_event(temp_button) == SINGLE_CLICK){
            vEV3P035JPlay(BE_VOICE);
            vPARA_SAVE();
            runState.disSetTimeState = DISSETTIME_STATE_NULL;
            ClearDisSetTimeBlinkCnt();
            runState.disIndex = DISINDEX_RUNNING;
            runState.bklight_time = GetConfigData_BledTime()*10;
        }
        if (temp_button->button_id == btnMenu_id && get_button_event(temp_button) == SINGLE_CLICK){
            vEV3P035JPlay(BE_VOICE);
            SetDisSetTimeState();
        }
        if (temp_button->button_id == btnMenu_id && get_button_event(temp_button) == LONG_PRESS_START){
            vEV3P035JPlay(BE_VOICE);
            vPARA_SAVE();
            runState.disSetTimeState = DISSETTIME_STATE_NULL;
            ClearDisSetTimeBlinkCnt();
            runState.disIndex = DISINDEX_RUNNING;
            runState.bklight_time = GetConfigData_BledTime()*10;
        }
        //加减按键操作
        if (temp_button->button_id == btnDec_id && get_button_event(temp_button) == PRESS_DOWN)
        {
            vEV3P035JPlay(BE_VOICE);
            SetDisSetTimeDec();
        }
        if (temp_button->button_id == btnAdd_id && get_button_event(temp_button) == PRESS_DOWN)
        {
            vEV3P035JPlay(BE_VOICE);
            SetDisSetTimeAdd();
        }
        break;
//    case DISINDEX_CFGCTRLPARAM_OILCASL_FULL://手动将油箱显示置满
//        if (temp_button->button_id == btnPower_id && get_button_event(temp_button) == SINGLE_CLICK){
//            vEV3P035JPlay(BE_VOICE);
//            vPARA_SAVE();
//            vClearScreen();
//            runState.disSetTimeState = DISSETTIME_STATE_NULL;
//            ClearDisSetTimeBlinkCnt();
//            runState.disIndex = DISINDEX_RUNNING;
//            runState.bklight_time = GetConfigData_BledTime()*10;
//        }
////        if (temp_button->button_id == btnEnt_id && get_button_event(temp_button) == SINGLE_CLICK){
////            vEV3P035JPlay(BE_VOICE);
////            SetDisSetTimeState();
////        }
//        if (temp_button->button_id == btnMenu_id && get_button_event(temp_button) == LONG_PRESS_START){
//            vEV3P035JPlay(BE_VOICE);
//            //置满油箱
//            runState.resOilMass = (uint32_t)(baseParameters.oilCaseVolume) * (uint32_t)1000000;
//        }
//        //加减按键操作
//        if (temp_button->button_id == btnDec_id && get_button_event(temp_button) == PRESS_DOWN)
//        {
//            vEV3P035JPlay(BE_VOICE);
//            SetDisSetTimeDec();
//        }
//        if (temp_button->button_id == btnAdd_id && get_button_event(temp_button) == PRESS_DOWN)
//        {
//            vEV3P035JPlay(BE_VOICE);
//            SetDisSetTimeAdd();
//        }
//        break;
    }
}

//按键IO初始化
void user_button_init(void)
{
    GPIOA_ModeCfg(GPIO_Pin_0 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6, GPIO_ModeIN_PU);

    button_init(&btnPower, read_button_GPIO, 0, btnPower_id);
    button_init(&btnMenu, read_button_GPIO, 0, btnMenu_id);
    button_init(&btnAdd, read_button_GPIO, 0, btnAdd_id);
    button_init(&btnDec, read_button_GPIO, 0, btnDec_id);
    button_init(&btnEng, read_button_GPIO, 0, btnEng_id);

    button_attach(&btnPower, SINGLE_CLICK, btn_event_callback); //单击
//    button_attach(&btnPower, DOUBLE_CLICK, btn_event_callback); //双击
    button_attach(&btnPower, LONG_PRESS_START, btn_event_callback);//到长按阈值触发一次长按
    button_attach(&btnPower, LONG_PRESS_HOLD, btn_event_callback);//长按一直触发
    button_attach(&btnPower, PRESS_DOWN, btn_event_callback);//按下
    button_attach(&btnPower, PRESS_UP, btn_event_callback);//抬起

    button_attach(&btnMenu, SINGLE_CLICK, btn_event_callback); //单击
//    button_attach(&btnMenu, DOUBLE_CLICK, btn_event_callback); //双击
    button_attach(&btnMenu, LONG_PRESS_START, btn_event_callback);
    button_attach(&btnMenu, LONG_PRESS_HOLD, btn_event_callback);
    button_attach(&btnMenu, PRESS_DOWN, btn_event_callback);
//    button_attach(&btnMenu, PRESS_UP, btn_event_callback);

    button_attach(&btnAdd, SINGLE_CLICK, btn_event_callback); //单击
//    button_attach(&btnAdd, DOUBLE_CLICK, btn_event_callback); //双击
    button_attach(&btnAdd, LONG_PRESS_START, btn_event_callback);
    button_attach(&btnAdd, LONG_PRESS_HOLD, btn_event_callback);
    button_attach(&btnAdd, PRESS_DOWN, btn_event_callback);

    button_attach(&btnDec, SINGLE_CLICK, btn_event_callback); //单击
//    button_attach(&btnDec, DOUBLE_CLICK, btn_event_callback); //双击
    button_attach(&btnDec, LONG_PRESS_START, btn_event_callback);
    button_attach(&btnDec, LONG_PRESS_HOLD, btn_event_callback);
    button_attach(&btnDec, PRESS_DOWN, btn_event_callback);

    button_attach(&btnEng, LONG_PRESS_START, btn_event_callback);//到长按阈值触发一次长按

    button_start(&btnPower);
    button_start(&btnMenu);
    button_start(&btnAdd);
    button_start(&btnDec);
    button_start(&btnEng);

    //定时器0初始化 20ms
    TMR0_TimerInit(FREQ_SYS / 50);
    TMR0_ITCfg(ENABLE, TMR0_3_IT_CYC_END);
    PFIC_EnableIRQ(TMR0_IRQn);
}

/*********************************************************************
 * @fn      TMR0_IRQHandler
 *btn_event_callback
 * @brief   TMR0中断函数
 *
 * @return  none
 */
__INTERRUPT
__HIGH_CODE
void TMR0_IRQHandler(void) // TMR0 定时中断
{
    if (TMR0_GetITFlag(TMR0_3_IT_CYC_END))
    {
        button_ticks();
        TMR0_ClearITFlag(TMR0_3_IT_CYC_END); // 清除中断标志
    }
}
