/*
 * running.c
 *
 *  Created on: 2023骞�6鏈�21鏃�
 *      Author: wx030
 */

#include "HAL.h"

Run_Handle_t runState={0};
//uint8_t automaticheating = 0;
//uint8_t writed = 0;
//uint8_t ucmd = 0;

void runState_Init(void)
{
    memset(&runState,0,sizeof(runState));
    memset(&heaterDatas,0,sizeof(heaterDatas));
    runState.disIndex = DISINDEX_NULL;
    runState.remoteMatchState =DISSETENGMODE_REMOTEMATCH_NULL;
    runState.auto_setLevel = 3;
    runState.setLevel = 3;
    runState.setTemper = 25;
    runState.setHTemper = 77;       //华氏摄氏度
    runState.ventilateLevel = POWER_LEVEL_NUM_MIN;
    runState.runModel = REMOTE_STATE_POWER_OFF;
    runState.firstPowerOn = 0;
    runState.offingBlinkTime = OFFING_BLINK_TIME;
    runState.autoOnToOff = FALSE;
    runState.resOilMass = (uint32_t)(baseParameters.oilCaseVolume) * (uint32_t)1000000;
    runState.bleSetPassword = 0;
    runState.temperStandbyState = TEMPER_STADNBY_STATE_NULL;
    runState.mainBoardType = MAIN_BOARD_TYPE_NONE_VENTILATE;    //默认不带通风强热，等第一次通信后获取主板类型再修改
    runState.standbyOnOffDisState = TEMPER_STADNBY_STATE_NULL;
    runState.bledDuty = BLED_DUTY_MAX;//baseParameters.bledDuty;
    runState.ctrlCmdFlag = 0;
    runState.pumPoilFlag = 0;
    baseParameters.runMode = CTRL_RUNMODE_LEVEL;

    heaterDatas.runningStep = HEATERRUNSTEP_NULL;
    if(GetDisSetTemperModeStandby())
    {
        SetConfigData_RunMode(CTRL_RUNMODE_TEMP);
    }

    runState.vccAdcMax = 2500;
    runState.vccAdcMin = 3145;
    runState.vccAdcDiffer = 0;
}

//关机清除各状态
void clear_State(void)
{
    runState.runModel = REMOTE_STATE_POWER_OFF;
    runState.disIndex = DISINDEX_PWROFFED;
    heaterDatas.runningStep = HEATERRUNSTEP_NULL;
    heaterDatas.faultType = Fault_Null;
    runState.bleSetPassword = 0;
    heaterDatas.sensorType = 0;
}

//获取手动设定风量
uint8_t GetRunningSetLevel(void)
{
    return runState.setLevel;
}

//获取自动设定风量
uint8_t GetRunningAutoSetLevel(void)
{
    return runState.auto_setLevel;
}

void SetRunningSetLevel(uint8 level)
{
    if(level<POWER_LEVEL_NUM_MIN)    runState.setLevel = POWER_LEVEL_NUM_MIN;
    else  if(level>POWER_LEVEL_NUM_MAX)    runState.setLevel = POWER_LEVEL_NUM_MAX;
    else runState.setLevel = level;
}

uint8_t GetRunningAutoLevel(void)
{
    return runState.auto_Level;
}

void SetRunningAutoLevel(uint8 level)
{
    if(level<POWER_LEVEL_NUM_MIN)    runState.auto_Level = POWER_LEVEL_NUM_MIN;
    else  if(level>POWER_LEVEL_NUM_MAX)    runState.auto_Level = POWER_LEVEL_NUM_MAX;
    else runState.auto_Level = level;
}

void SetRunningAutoSetLevel(uint8 level)
{
    if(level<POWER_LEVEL_NUM_MIN)    runState.auto_setLevel = POWER_LEVEL_NUM_MIN;
    else  if(level>POWER_LEVEL_NUM_MAX)    runState.auto_setLevel = POWER_LEVEL_NUM_MAX;
    else runState.auto_setLevel = level;
}

void RunningSetLevel_Add(void)
{
    runState.setLevel++;
    if(runState.setLevel>POWER_LEVEL_NUM_MAX)    runState.setLevel=POWER_LEVEL_NUM_MAX;
}

void RunningSetLevel_Dec(void)
{
    runState.setLevel--;
    if(runState.setLevel<POWER_LEVEL_NUM_MIN)    runState.setLevel=POWER_LEVEL_NUM_MIN;
}

//获取设定温度
int16_t GetRunningSetTemper(void)
{
    return runState.setTemper;
}

int16_t GetRunningSetHTemper(void)
{
    return runState.setHTemper;
}

void SetRunningSetTemper(uint16 temp)
{
    if(temp<CTRL_RUNMODE_TEMP_MIN)  runState.setTemper=CTRL_RUNMODE_TEMP_MIN;
    else if(temp>CTRL_RUNMODE_TEMP_MAX)  runState.setTemper=CTRL_RUNMODE_TEMP_MAX;
    else runState.setTemper = temp;
}

void RunningSetTemper_Add(void)
{
    if(baseParameters.tempUnit == TEMPER_UINT_C)
    {
        runState.setTemper++;
        if(runState.setTemper>CTRL_RUNMODE_TEMP_MAX)    runState.setTemper=CTRL_RUNMODE_TEMP_MAX;

        runState.setHTemper = (int16_t)(((float)runState.setTemper*1.8) + 32.0);
        if(runState.setHTemper>CTRL_RUNMODE_HTEMP_MAX)  runState.setHTemper = CTRL_RUNMODE_HTEMP_MAX;
        if(runState.setHTemper<CTRL_RUNMODE_HTEMP_MIN)  runState.setHTemper = CTRL_RUNMODE_HTEMP_MIN;
    }else if(baseParameters.tempUnit == TEMPER_UINT_F)
    {
        runState.setHTemper++;
        if(runState.setHTemper>CTRL_RUNMODE_HTEMP_MAX)    runState.setHTemper=CTRL_RUNMODE_HTEMP_MAX;

        runState.setTemper = (int16_t)(((float)(runState.setHTemper-32))/1.8);
    }
}

void RunningSetTemper_Dec(void)
{
    if(baseParameters.tempUnit == TEMPER_UINT_C)
    {
        runState.setTemper--;
        if(runState.setTemper<CTRL_RUNMODE_TEMP_MIN)    runState.setTemper=CTRL_RUNMODE_TEMP_MIN;

        runState.setHTemper = (int16_t)(((float)runState.setTemper*1.8) + 32.0);
        if(runState.setHTemper>CTRL_RUNMODE_HTEMP_MAX)  runState.setHTemper = CTRL_RUNMODE_HTEMP_MAX;
        if(runState.setHTemper<CTRL_RUNMODE_HTEMP_MIN)  runState.setHTemper = CTRL_RUNMODE_HTEMP_MIN;
    }else if(baseParameters.tempUnit == TEMPER_UINT_F)
    {
        runState.setHTemper--;
        if(runState.setHTemper<CTRL_RUNMODE_HTEMP_MIN)    runState.setHTemper=CTRL_RUNMODE_HTEMP_MIN;

        runState.setTemper = (int16_t)(((float)(runState.setHTemper-32))/1.8);
    }
}

//通风档位+
void RunningVentilateLevel_Add(void)
{
    runState.ventilateLevel++;
    if(runState.ventilateLevel>POWER_LEVEL_NUM_MAX)    runState.ventilateLevel=POWER_LEVEL_NUM_MAX;
}
//通风档位－
void RunningVentilateLevel_Dec(void)
{
    runState.ventilateLevel--;
    if(runState.ventilateLevel<POWER_LEVEL_NUM_MIN)    runState.ventilateLevel=POWER_LEVEL_NUM_MIN;
}
//设置通风档位
void SetRunningVentilateLevel(uint8_t level)
{
    if(level<POWER_LEVEL_NUM_MIN)           runState.ventilateLevel = POWER_LEVEL_NUM_MIN;
    else  if(level>POWER_LEVEL_NUM_MAX)     runState.ventilateLevel = POWER_LEVEL_NUM_MAX;
    else runState.ventilateLevel = level;
}
//获取通风档位
uint8_t GetRunningVentilateLevel(void)
{
    return runState.ventilateLevel;
}

//切换控制模式：温空模式、手动模式、自动启停模式
void ControlModel_Change(void)
{
    struct tm time;
    //点亮背光
    runState.bklight_time = GetConfigData_BledTime()*10;

    runState.ctrlCmdFlag = 1;

    baseParameters.runMode ++;
    if(runState.mainBoardType == MAIN_BOARD_TYPE_NONE_VENTILATE)
    {
        //无通风功能主机
        if(baseParameters.runMode > CTRL_RUNMODE_LEVEL)    baseParameters.runMode = CTRL_RUNMODE_TEMP;
    }else if(runState.mainBoardType == MAIN_BOARD_TYPE_VENTILATE_FASTHART){
        //有通风功能主机
        if(baseParameters.runMode >= CTRL_RUNMODE_MAX)    baseParameters.runMode = CTRL_RUNMODE_TEMP;
    }
    switch(baseParameters.runMode)
    {
    case CTRL_RUNMODE_TEMP://温度模式
        vEV3P035JPlay(TEMP_MODE);

        SetDisSetTemperBlink(1);
        //若是自动启停关机状态下，开机
        if(runState.temperStandbyState == TEMPER_STADNBY_STATE_OFF)
        {
            runState.temperStandbyState = TEMPER_STADNBY_STATE_NULL;
            runState.standbyOnOffDisState = TEMPER_STADNBY_STATE_NULL;
        }
        break;
    case CTRL_RUNMODE_LEVEL://手动模式
        vEV3P035JPlay(LEVEL_MODE);

        //若是自动启停关机状态下，开机
        if(runState.temperStandbyState == TEMPER_STADNBY_STATE_OFF)
        {
            runState.temperStandbyState = TEMPER_STADNBY_STATE_NULL;
            runState.standbyOnOffDisState = TEMPER_STADNBY_STATE_NULL;
            runState.disIndex = DISINDEX_PWRON_ANIMATION;//切换开机动画
            //播放开始加热
//            vEV3P035JPlay(START_HEAT);
        }
        break;
    case CTRL_RUNMODE_VENTILATE://通风
        if(runState.temperStandbyState == TEMPER_STADNBY_STATE_OFF)
        {
            runState.temperStandbyState = TEMPER_STADNBY_STATE_NULL;
            runState.standbyOnOffDisState = TEMPER_STADNBY_STATE_NULL;
        }
        break;
    case CTRL_RUNMODE_FASTHEAT:
        //存储当前强热开启时间
        time = gettimebyoffset();
        runState.fastHeatStartMinute = (time.tm_hour*60) + time.tm_min;
        if(runState.temperStandbyState == TEMPER_STADNBY_STATE_OFF)
        {
            runState.temperStandbyState = TEMPER_STADNBY_STATE_NULL;
            runState.standbyOnOffDisState = TEMPER_STADNBY_STATE_NULL;
            runState.disIndex = DISINDEX_PWRON_ANIMATION;//切换开机动画
        }
        break;
    default:
        break;
    }
}
//获取热敏电阻温度
int16_t GetTemperatureValue(void)
{
    return heaterDatas.cabTemp;
}

uint8_t GetDisSetTemperBlink(void)
{
    return runState.disSetTemp;
}

void CleardisSetTempBlinkCnt(void)
{
    runState.disSetTempBlinkCnt = 0;
}

void SetDisSetTemperBlink(uint8_t state)
{
    if(state >= 1) runState.disSetTemp = 1;
    else runState.disSetTemp = 0;
}

uint8_t GetDisSetTimeState(void)
{
    return runState.disSetTimeState;
}

void ClearDisSetTimeBlinkCnt(void)
{
    runState.disSetTimeBlinkCnt = 0;
}

void SetDisSetTimeState(void)
{
    switch(runState.disIndex)
    {
    case DISINDEX_CFGCTRLPARAM_RTC:
    case DISINDEX_CFGCTRLPARAM_TIMER:
        runState.disSetTimeState ++;
        if(runState.disSetTimeState>DISSETTIME_STATE_MINUTE)
        {
            runState.disSetTimeState = DISSETTIME_STATE_NULL;
        }
        break;
    case DISINDEX_CFGCTRLPARAM_TIMER_RUNTIME:
        runState.disSetTimeState = (runState.disSetTimeState==DISSETTIME_STATE_NULL) ? DISSETTIME_STATE_HOUR_MINUTE:DISSETTIME_STATE_NULL;
        break;
    case DISINDEX_CFGCTRLPARAM_TIMER_SW:
        runState.disSetTimeState = (runState.disSetTimeState==DISSETTIME_STATE_NULL) ? DISSETTIME_STATE_MINUTE:DISSETTIME_STATE_NULL;
        break;
//    case DISINDEX_CFGCTRLPARAM_VOICE_SW:
//        runState.disSetTimeState = (runState.disSetTimeState==DISSETTIME_STATE_NULL) ? DISSETTIME_STATE_MINUTE:DISSETTIME_STATE_NULL;
//        break;
    case DISINDEX_CFGCTRLPARAM_TEMPEROFFSET:
        runState.disSetTimeState = (runState.disSetTimeState==DISSETTIME_STATE_NULL) ? DISSETTIME_STATE_HOUR_MINUTE:DISSETTIME_STATE_NULL;
        break;
//    case DISINDEX_CFGCTRLPARAM_OILCASL_VOLUME:
//        runState.disSetTimeState = (runState.disSetTimeState==DISSETTIME_STATE_NULL) ? DISSETTIME_STATE_HOUR_MINUTE:DISSETTIME_STATE_NULL;
//        break;
//    case DISINDEX_CFGCTRLPARAM_OILPUMP_TYPE:
//        runState.disSetTimeState = (runState.disSetTimeState==DISSETTIME_STATE_NULL) ? DISSETTIME_STATE_HOUR_MINUTE:DISSETTIME_STATE_NULL;
//        break;
    case DISINDEX_CFGCTRLPARAM_BLE_ONOFF:
        runState.disSetTimeState = (runState.disSetTimeState==DISSETTIME_STATE_NULL) ? DISSETTIME_STATE_MINUTE:DISSETTIME_STATE_NULL;
        break;
    case DISINDEX_CFGCTRLPARAM_433_ONOFF:
        runState.disSetTimeState = (runState.disSetTimeState==DISSETTIME_STATE_NULL) ? DISSETTIME_STATE_MINUTE:DISSETTIME_STATE_NULL;
        break;
    case DISINDEX_CFGCTRLPARAM_TEMPER_STANDBY_SW:
        runState.disSetTimeState = (runState.disSetTimeState==DISSETTIME_STATE_NULL) ? DISSETTIME_STATE_MINUTE:DISSETTIME_STATE_NULL;
        break;
    case DISINDEX_CFGCTRLPARAM_C_TO_F:
        runState.disSetTimeState = (runState.disSetTimeState==DISSETTIME_STATE_NULL) ? DISSETTIME_STATE_MINUTE:DISSETTIME_STATE_NULL;
        break;
    case DISINDEX_CFGCTRLPARAM_BLED_CTL:
        runState.disSetTimeState = (runState.disSetTimeState==DISSETTIME_STATE_NULL) ? DISSETTIME_STATE_MINUTE:DISSETTIME_STATE_NULL;
        break;
    case DISINDEX_CFG_433CO_OFF://433接收CO超标固定关机码
        runState.disSetTimeState = (runState.disSetTimeState==DISSETTIME_STATE_NULL) ? DISSETTIME_STATE_MINUTE:DISSETTIME_STATE_NULL;
        break;
#if CO_VALUE==1
    case DISINDEX_CFGCTRLPARAM_CO_WARNING:
        runState.disSetTimeState = (runState.disSetTimeState==DISSETTIME_STATE_NULL) ? DISSETTIME_STATE_MINUTE:DISSETTIME_STATE_NULL;
        break;
#endif
    default:
        break;
    }
    if(runState.disSetTimeState != DISSETTIME_STATE_NULL)
    {
        runState.disSetTimeBlinkCnt = DISP_BLINK_FREQUENCY/2;
    }else   ClearDisSetTimeBlinkCnt();
}

//参数设置页面--页面显示+
void SetDisRunState_disIndexAdd(void)
{
    runState.disIndex++;
    if(runState.disIndex >= DISINDEX_CFGCTRLPARAM_MAX) runState.disIndex = DISINDEX_CFGCTRLPARAM_RTC;
}
//参数设置页面--页面显示-
void SetDisRunState_disIndexDec(void)
{
    runState.disIndex--;
    if(runState.disIndex < DISINDEX_CFGCTRLPARAM_RTC) runState.disIndex = DISINDEX_CFGCTRLPARAM_MAX-1;
}

//工程模式+
void SetDisEngModeAdd(void)
{
    runState.disIndex++;
    if(runState.disIndex >= DISINDEX_ENGMODE_MAX) runState.disIndex = DISINDEX_ENGMODE_HEATERFW;
}
//工程模式-
void SetDisEngModeDec(void)
{
    runState.disIndex--;
    if(runState.disIndex < DISINDEX_ENGMODE_HEATERFW) runState.disIndex = DISINDEX_ENGMODE_MAX-1;
}

//参数设置加
void SetDisSetTimeAdd(void)
{
    switch(runState.disIndex)
    {
    case DISINDEX_CFGCTRLPARAM_RTC:
        switch(GetDisSetTimeState())
        {
        case DISSETTIME_STATE_NULL:
            SetDisRunState_disIndexAdd();
            break;
        case DISSETTIME_STATE_HOUR:
            ClearDisSetTimeBlinkCnt();
            SetDisSetTimeHourAdd();
            break;
        case DISSETTIME_STATE_MINUTE:
            ClearDisSetTimeBlinkCnt();
            SetDisSetTimeMinuteAdd();
            break;
        }
        break;
    case DISINDEX_CFGCTRLPARAM_TIMER:
        switch(GetDisSetTimeState())
        {
        case DISSETTIME_STATE_NULL:
            SetDisRunState_disIndexAdd();
            break;
        case DISSETTIME_STATE_HOUR:
            ClearDisSetTimeBlinkCnt();
            SetDisSetTimerHourAdd();
            break;
        case DISSETTIME_STATE_MINUTE:
            ClearDisSetTimeBlinkCnt();
            SetDisSetTimerMinuteAdd();
            break;
        }
        break;
    case DISINDEX_CFGCTRLPARAM_TIMER_RUNTIME:
        switch(GetDisSetTimeState())
        {
        case DISSETTIME_STATE_NULL:
            SetDisRunState_disIndexAdd();
            break;
        case DISSETTIME_STATE_HOUR_MINUTE:
            ClearDisSetTimeBlinkCnt();
            SetDisSetTimerRunTimeAdd30();
            break;
        }
        break;
    case DISINDEX_CFGCTRLPARAM_TIMER_SW:
        switch(GetDisSetTimeState())
        {
        case DISSETTIME_STATE_NULL:
            SetDisRunState_disIndexAdd();
            break;
        case DISSETTIME_STATE_MINUTE:
            ClearDisSetTimeBlinkCnt();
            if(GetDisSetTimerClockEn()) SetDisSetTimerClockEn(0);
            else SetDisSetTimerClockEn(1);
            break;
        }
        break;
//    case DISINDEX_CFGCTRLPARAM_VOICE_SW:
//        switch(GetDisSetTimeState())
//        {
//        case DISSETTIME_STATE_NULL:
//            SetDisRunState_disIndexAdd();
//            break;
//        case DISSETTIME_STATE_MINUTE:
//            ClearDisSetTimeBlinkCnt();
//            SetDisSetLanguageTypeAdd();
//            break;
//        }
//        break;
    case DISINDEX_CFGCTRLPARAM_TEMPEROFFSET:
        switch(GetDisSetTimeState())
        {
        case DISSETTIME_STATE_NULL:
            SetDisRunState_disIndexAdd();
            break;
        case DISSETTIME_STATE_HOUR_MINUTE:
            ClearDisSetTimeBlinkCnt();
            SetDisSetTemperOffSetAdd();
            break;
        }
        break;
//    case DISINDEX_CFGCTRLPARAM_OILCASL_VOLUME:
//        switch(GetDisSetTimeState())
//        {
//        case DISSETTIME_STATE_NULL:
//            SetDisRunState_disIndexAdd();
//            break;
//        case DISSETTIME_STATE_HOUR_MINUTE:
//            ClearDisSetTimeBlinkCnt();
//            SetDisSetOilCaseVolumeAdd5();
//            break;
//        }
//    break;
//    case DISINDEX_CFGCTRLPARAM_OILPUMP_TYPE:
//        switch(GetDisSetTimeState())
//        {
//        case DISSETTIME_STATE_NULL:
//            SetDisRunState_disIndexAdd();
//            break;
//        case DISSETTIME_STATE_HOUR_MINUTE:
//            ClearDisSetTimeBlinkCnt();
//            SetDisSetOilPumpTypeAdd();
//            break;
//        }
//        break;
    case DISINDEX_CFGCTRLPARAM_BLE_ONOFF:
        switch(GetDisSetTimeState())
        {
        case DISSETTIME_STATE_NULL:
            SetDisRunState_disIndexAdd();
            break;
        case DISSETTIME_STATE_MINUTE:
            ClearDisSetTimeBlinkCnt();
            if(GetDisSetBleOnOff()==TRUE)   SetDisSetBleOnOff(0);
            else                            SetDisSetBleOnOff(1);
            break;
        }
        break;
    case DISINDEX_CFGCTRLPARAM_433_ONOFF:
        switch(GetDisSetTimeState())
        {
        case DISSETTIME_STATE_NULL:
            SetDisRunState_disIndexAdd();
            break;
        case DISSETTIME_STATE_MINUTE:
            ClearDisSetTimeBlinkCnt();
            if(GetDisSet433OnOff()==TRUE)   SetDisSet433OnOff(0);
            else                            SetDisSet433OnOff(1);
            break;
        }
        break;
    case DISINDEX_CFGCTRLPARAM_TEMPER_STANDBY_SW:
        switch(GetDisSetTimeState())
        {
        case DISSETTIME_STATE_NULL:
            SetDisRunState_disIndexAdd();
            break;
        case DISSETTIME_STATE_MINUTE:
            ClearDisSetTimeBlinkCnt();
            if(GetDisSetTemperModeStandby()==TRUE)  SetDisSetTemperModeStandby(0);
            else                                    SetDisSetTemperModeStandby(1);
            break;
        }
        break;
    case DISINDEX_CFGCTRLPARAM_C_TO_F:
        switch(GetDisSetTimeState())
        {
        case DISSETTIME_STATE_NULL:
            SetDisRunState_disIndexAdd();
            break;
        case DISSETTIME_STATE_MINUTE:
            ClearDisSetTimeBlinkCnt();
            SetDisSetTemperUintAdd();
            break;
        }
        break;
    case DISINDEX_CFGCTRLPARAM_BLED_CTL:
        switch(GetDisSetTimeState())
        {
        case DISSETTIME_STATE_NULL:
            SetDisRunState_disIndexAdd();
            break;
        case DISSETTIME_STATE_MINUTE:
            ClearDisSetTimeBlinkCnt();
            SetConfigData_BledDuty_Add();
            break;
        }
        break;
    case DISINDEX_CFG_433CO_OFF:
        switch(GetDisSetTimeState())
        {
        case DISSETTIME_STATE_NULL:
            SetDisRunState_disIndexAdd();
            break;
        case DISSETTIME_STATE_MINUTE:
            ClearDisSetTimeBlinkCnt();
            if(baseParameters.co433Off==TRUE)   baseParameters.co433Off = FALSE;
            else                                baseParameters.co433Off = TRUE;
            break;
        }
        break;
#if CO_VALUE==1
    case DISINDEX_CFGCTRLPARAM_CO_WARNING:
        switch(GetDisSetTimeState())
        {
        case DISSETTIME_STATE_NULL:
            SetDisRunState_disIndexAdd();
            break;
        case DISSETTIME_STATE_MINUTE:
            ClearDisSetTimeBlinkCnt();
            SetConfigData_COWarningData_Add();
            break;
        }
        break;
#endif
//        case DISINDEX_CFGCTRLPARAM_OILCASL_FULL:
//            SetDisRunState_disIndexAdd();
//            break;
    default:
        break;
    }
}
//参数设置减
void SetDisSetTimeDec(void)
{
    switch(runState.disIndex)
    {
    case DISINDEX_CFGCTRLPARAM_RTC:
        switch(GetDisSetTimeState())
        {
        case DISSETTIME_STATE_NULL:
            SetDisRunState_disIndexDec();
            break;
        case DISSETTIME_STATE_HOUR:
            ClearDisSetTimeBlinkCnt();
            SetDisSetTimeHourDec();
            break;
        case DISSETTIME_STATE_MINUTE:
            ClearDisSetTimeBlinkCnt();
            SetDisSetTimeMinuteDec();
            break;
        }
        break;
    case DISINDEX_CFGCTRLPARAM_TIMER:
        switch(GetDisSetTimeState())
        {
        case DISSETTIME_STATE_NULL:
            SetDisRunState_disIndexDec();
            break;
        case DISSETTIME_STATE_HOUR:
            ClearDisSetTimeBlinkCnt();
            SetDisSetTimerHourDec();
            break;
        case DISSETTIME_STATE_MINUTE:
            ClearDisSetTimeBlinkCnt();
            SetDisSetTimerMinuteDec();
            break;
        }
        break;
    case DISINDEX_CFGCTRLPARAM_TIMER_RUNTIME:
        switch(GetDisSetTimeState())
        {
        case DISSETTIME_STATE_NULL:
            SetDisRunState_disIndexDec();
            break;
        case DISSETTIME_STATE_HOUR_MINUTE:
            ClearDisSetTimeBlinkCnt();
            SetDisSetTimerRunTimeDec30();
            break;
        }
        break;
    case DISINDEX_CFGCTRLPARAM_TIMER_SW:
        switch(GetDisSetTimeState())
        {
        case DISSETTIME_STATE_NULL:
            SetDisRunState_disIndexDec();
            break;
        case DISSETTIME_STATE_MINUTE:
            ClearDisSetTimeBlinkCnt();
            if(GetDisSetTimerClockEn()) SetDisSetTimerClockEn(0);
            else SetDisSetTimerClockEn(1);
            break;
        }
        break;
//    case DISINDEX_CFGCTRLPARAM_VOICE_SW:
//        switch(GetDisSetTimeState())
//        {
//        case DISSETTIME_STATE_NULL:
//            SetDisRunState_disIndexDec();
//            break;
//        case DISSETTIME_STATE_MINUTE:
//            ClearDisSetTimeBlinkCnt();
//            SetDisSetLanguageTypeDec();
//            break;
//        }
//        break;
    case DISINDEX_CFGCTRLPARAM_TEMPEROFFSET:
        switch(GetDisSetTimeState())
        {
        case DISSETTIME_STATE_NULL:
            SetDisRunState_disIndexDec();
            break;
        case DISSETTIME_STATE_HOUR_MINUTE:
            ClearDisSetTimeBlinkCnt();
            SetDisSetTemperOffSetDec();
            break;
        }
        break;
//    case DISINDEX_CFGCTRLPARAM_OILCASL_VOLUME:
//        switch(GetDisSetTimeState())
//        {
//        case DISSETTIME_STATE_NULL:
//            SetDisRunState_disIndexDec();
//            break;
//        case DISSETTIME_STATE_HOUR_MINUTE:
//            ClearDisSetTimeBlinkCnt();
//            SetDisSetOilCaseVolumeDec5();
//            break;
//        }
//        break;
//    case DISINDEX_CFGCTRLPARAM_OILPUMP_TYPE:
//        switch(GetDisSetTimeState())
//        {
//        case DISSETTIME_STATE_NULL:
//            SetDisRunState_disIndexDec();
//            break;
//        case DISSETTIME_STATE_HOUR_MINUTE:
//            ClearDisSetTimeBlinkCnt();
//            SetDisSetOilPumpTypeDec();
//            break;
//        }
//        break;
    case DISINDEX_CFGCTRLPARAM_BLE_ONOFF:
        switch(GetDisSetTimeState())
        {
        case DISSETTIME_STATE_NULL:
            SetDisRunState_disIndexDec();
            break;
        case DISSETTIME_STATE_MINUTE:
            ClearDisSetTimeBlinkCnt();
            if(GetDisSetBleOnOff()==TRUE)   SetDisSetBleOnOff(0);
            else                            SetDisSetBleOnOff(1);
            break;
        }
        break;
    case DISINDEX_CFGCTRLPARAM_433_ONOFF:
        switch(GetDisSetTimeState())
        {
        case DISSETTIME_STATE_NULL:
            SetDisRunState_disIndexDec();
            break;
        case DISSETTIME_STATE_MINUTE:
            ClearDisSetTimeBlinkCnt();
            if(GetDisSet433OnOff()==TRUE)   SetDisSet433OnOff(0);
            else                            SetDisSet433OnOff(1);
            break;
        }
        break;
    case DISINDEX_CFGCTRLPARAM_TEMPER_STANDBY_SW:
        switch(GetDisSetTimeState())
        {
        case DISSETTIME_STATE_NULL:
            SetDisRunState_disIndexDec();
            break;
        case DISSETTIME_STATE_MINUTE:
            ClearDisSetTimeBlinkCnt();
            if(GetDisSetTemperModeStandby()==TRUE)  SetDisSetTemperModeStandby(0);
            else                                    SetDisSetTemperModeStandby(1);
            break;
        }
        break;
    case DISINDEX_CFGCTRLPARAM_C_TO_F:
        switch(GetDisSetTimeState())
        {
        case DISSETTIME_STATE_NULL:
            SetDisRunState_disIndexDec();
            break;
        case DISSETTIME_STATE_MINUTE:
            ClearDisSetTimeBlinkCnt();
            SetDisSetTemperUintDec();
            break;
        }
        break;
    case DISINDEX_CFGCTRLPARAM_BLED_CTL:
        switch(GetDisSetTimeState())
        {
        case DISSETTIME_STATE_NULL:
            SetDisRunState_disIndexDec();
            break;
        case DISSETTIME_STATE_MINUTE:
            ClearDisSetTimeBlinkCnt();
            SetConfigData_BledDuty_Dec();
            break;
        }
        break;
    case DISINDEX_CFG_433CO_OFF:
        switch(GetDisSetTimeState())
        {
        case DISSETTIME_STATE_NULL:
            SetDisRunState_disIndexDec();
            break;
        case DISSETTIME_STATE_MINUTE:
            ClearDisSetTimeBlinkCnt();
            if(baseParameters.co433Off==TRUE)   baseParameters.co433Off = FALSE;
            else                                baseParameters.co433Off = TRUE;
            break;
        }
        break;
#if CO_VALUE==1
    case DISINDEX_CFGCTRLPARAM_CO_WARNING:
        switch(GetDisSetTimeState())
        {
        case DISSETTIME_STATE_NULL:
            SetDisRunState_disIndexDec();
            break;
        case DISSETTIME_STATE_MINUTE:
            ClearDisSetTimeBlinkCnt();
            SetConfigData_COWarningData_Dec();
            break;
        }
        break;
#endif
//    case DISINDEX_CFGCTRLPARAM_OILCASL_FULL:
//        SetDisRunState_disIndexDec();
//        break;
    default:
        break;
    }
}

//当前时间小时+1
void SetDisSetTimeHourAdd(void)
{
    baseParameters.sttime.tm_hour++;
    if(baseParameters.sttime.tm_hour>23)    baseParameters.sttime.tm_hour=0;
    datetime_init();
}

//当前时间小时-1
void SetDisSetTimeHourDec(void)
{
    baseParameters.sttime.tm_hour--;
    if(baseParameters.sttime.tm_hour<0)    baseParameters.sttime.tm_hour=23;
    datetime_init();
}

//当前时间分钟+1
void SetDisSetTimeMinuteAdd(void)
{
    baseParameters.sttime.tm_min++;
    if(baseParameters.sttime.tm_min>59)    baseParameters.sttime.tm_min=0;
    datetime_init();
}

//当前时间分钟-1
void SetDisSetTimeMinuteDec(void)
{
    baseParameters.sttime.tm_min--;
    if(baseParameters.sttime.tm_min<0)    baseParameters.sttime.tm_min=59;
    datetime_init();
}

//设置当前时间
void SetDisSetTime(uint8 hour,uint8 minute)
{
    if(hour>23)     baseParameters.sttime.tm_hour = 0;
    else            baseParameters.sttime.tm_hour = hour;
    if(minute>59)   baseParameters.sttime.tm_min = 0;
    else            baseParameters.sttime.tm_min = minute;
    datetime_init();
}

//定时小时+1
void SetDisSetTimerHourAdd(void)
{
    baseParameters.timerClockHour++;
    if(baseParameters.timerClockHour>23)    baseParameters.timerClockHour=0;
}

//定时小时-1
void SetDisSetTimerHourDec(void)
{
    baseParameters.timerClockHour--;
    if(baseParameters.timerClockHour>23)    baseParameters.timerClockHour=23;
}

uint8_t GetDisSetTimerHour(void)
{
    return baseParameters.timerClockHour;
}

//定时分钟+1
void SetDisSetTimerMinuteAdd(void)
{
    baseParameters.timerClockMinute++;
    if(baseParameters.timerClockMinute>59)    baseParameters.timerClockMinute=0;
}

//定时分钟-1
void SetDisSetTimerMinuteDec(void)
{
    baseParameters.timerClockMinute--;
    if(baseParameters.timerClockMinute>59)    baseParameters.timerClockMinute=59;
}

uint8_t GetDisSetTimerMinute(void)
{
    return baseParameters.timerClockMinute;
}

//设置定时时间
void SetDisSetTimer(uint8 hour,uint8 minute)
{
    if(hour>23)     baseParameters.timerClockHour = 0;
    else            baseParameters.timerClockHour = hour;
    if(minute>59)   baseParameters.timerClockMinute = 0;
    else            baseParameters.timerClockMinute = minute;
}

void SetDisSetTimerRunTimeAdd30(void)
{
    baseParameters.totalHeatTime+=30;
    if(baseParameters.totalHeatTime>600)    baseParameters.totalHeatTime=0;
}

void SetDisSetTimerRunTimeDec30(void)
{
    baseParameters.totalHeatTime-=30;
    if(baseParameters.totalHeatTime>600)    baseParameters.totalHeatTime=600;
}

void SetDisSetTimerRunTime(uint16_t minute)
{
    if(minute>600)  baseParameters.totalHeatTime=0;
    else            baseParameters.totalHeatTime=minute;
}

uint16_t GetDisSetTimerRunTime(void)
{
    return baseParameters.totalHeatTime;
}

void SetDisSetTimerClockEn(uint8 en)
{
    if(en > 1)  baseParameters.timerClockEn = 1;
    else baseParameters.timerClockEn = en;
}

uint8_t GetDisSetTimerClockEn(void)
{
    return baseParameters.timerClockEn;
}

uint8_t GetDisSetLanguageType(void)
{
    return baseParameters.languageType;
}

void SetDisSetLanguageType(uint8_t type)
{
    if(type >= SPEECHLANGUARE_MAX)  baseParameters.languageType = SPEECHLANGUARE_OFF;
    else baseParameters.languageType = type;
}

void SetDisSetLanguageTypeAdd(void)
{
#if GERMAN==0
    baseParameters.languageType++;
    if(baseParameters.languageType >= SPEECHLANGUARE_MAX)   baseParameters.languageType = SPEECHLANGUARE_OFF;
#elif GERMAN==1
    baseParameters.languageType++;
    if(baseParameters.languageType >= SPEECHLANGUARE_MAX)   baseParameters.languageType = SPEECHLANGUARE_OFF;
#elif GERMAN==2
    baseParameters.languageType = (baseParameters.languageType==SPEECHLANGUARE_OFF) ? SPEECHLANGUARE_ENGLISH:SPEECHLANGUARE_OFF;
#endif
}

void SetDisSetLanguageTypeDec(void)
{
#if GERMAN==0
    baseParameters.languageType--;
    if(baseParameters.languageType >= SPEECHLANGUARE_MAX)   baseParameters.languageType = SPEECHLANGUARE_RUSSIAN;
#elif GERMAN==1
    baseParameters.languageType--;
    if(baseParameters.languageType >= SPEECHLANGUARE_MAX)   baseParameters.languageType = SPEECHLANGUARE_RUSSIAN;
#elif GERMAN==2
    baseParameters.languageType = (baseParameters.languageType==SPEECHLANGUARE_OFF) ? SPEECHLANGUARE_ENGLISH:SPEECHLANGUARE_OFF;
#endif
}

int8_t GetDisSetTemperOffSet(void)
{
    return baseParameters.temperSensorOffset;
}

void SetDisSetTemperOffSet(int8_t offset)
{
    if(offset > 9 || offset<-9)  baseParameters.temperSensorOffset = 0;
    else baseParameters.temperSensorOffset = offset;
}

void SetDisSetTemperOffSetAdd(void)
{
    baseParameters.temperSensorOffset++;
    if(baseParameters.temperSensorOffset>9) baseParameters.temperSensorOffset=9;
}

void SetDisSetTemperOffSetDec(void)
{
    baseParameters.temperSensorOffset--;
    if(baseParameters.temperSensorOffset<-9) baseParameters.temperSensorOffset=-9;
}

uint8_t GetDisSetOilCaseVolume(void)
{
    return baseParameters.oilCaseVolume;
}

void SetDisSetOilCaseVolume(uint8_t dat)
{
    if(dat%5==0 && dat<=50) baseParameters.oilCaseVolume = dat;
    else baseParameters.oilCaseVolume = 0;
}

void SetDisSetOilCaseVolumeAdd5(void)
{
    baseParameters.oilCaseVolume += 5;
    if(baseParameters.oilCaseVolume>50) baseParameters.oilCaseVolume = 50;
}

void SetDisSetOilCaseVolumeDec5(void)
{
    baseParameters.oilCaseVolume -= 5;
    if(baseParameters.oilCaseVolume>50) baseParameters.oilCaseVolume = 0;
}

uint8_t GetDisSetOilPumpType(void)
{
    return baseParameters.oilPumpType;
}

void SetDisSetOilPumpType(uint8_t type)
{
    if(type > OILPUMP_TYPE_32UL)    baseParameters.oilPumpType = OILPUMP_TYPE_32UL;
    else baseParameters.oilPumpType = type;
}

void SetDisSetOilPumpTypeAdd(void)
{
    baseParameters.oilPumpType++;
    if(baseParameters.oilPumpType > OILPUMP_TYPE_32UL)  baseParameters.oilPumpType = OILPUMP_TYPE_32UL;
}

void SetDisSetOilPumpTypeDec(void)
{
    baseParameters.oilPumpType--;
    if(baseParameters.oilPumpType < OILPUMP_TYPE_16UL)  baseParameters.oilPumpType = OILPUMP_TYPE_16UL;
}

void SetDisSetBleOnOff(uint8_t state)
{
    if(state>1) baseParameters.bleOnOff = 1;
    else baseParameters.bleOnOff = state;
}

uint8_t GetDisSetBleOnOff(void)
{
    return baseParameters.bleOnOff;
}

void SetDisSet433OnOff(uint8_t state)
{
    if(state>1) baseParameters.rf433OnOff = 1;
    else baseParameters.rf433OnOff = state;
}

uint8_t GetDisSet433OnOff(void)
{
    return baseParameters.rf433OnOff;
}

uint8_t GetDisSetTemperModeStandby(void)
{
    return baseParameters.temperModeStandby;
}

void SetDisSetTemperModeStandby(uint8_t state)
{
    if(state>1) baseParameters.temperModeStandby = 1;
    else baseParameters.temperModeStandby = state;

//    SetConfigData_RunMode(CTRL_RUNMODE_TEMP);

    //中途若修改了自动启停标志位，需要修改控制器运行模式
//    if(GetDisSetTemperModeStandby()==1)    //自动启停打开
//    {
//        SetConfigData_RunMode(CTRL_RUNMODE_TEMP);   //运行页面显示自动启停
//    }else
//    {
//        if(GetConfigData_RunMode()==CTRL_RUNMODE_AUTO)  SetConfigData_RunMode(CTRL_RUNMODE_TEMP);
//    }
}

void ClearDisSetRemoteMatchState(void)
{
    runState.disSetTimeState = DISSETTIME_STATE_NULL;
    runState.remoteMatchState = DISSETENGMODE_REMOTEMATCH_NULL;
}

//获取当前显示状态
uint8_t GetDisIndex(void)
{
    return runState.disIndex;
}

//判断定时是否开机
uint8_t LookupTimerPwrOn(uint8_t timer_hour,uint8_t timer_minute)
{
    uint16_t timerminute=0,currentminute=0;
    struct tm time;

    time = gettimebyoffset();
    timerminute = (timer_hour*60) + timer_minute;
    currentminute = (time.tm_hour*60) + time.tm_min;
    if(GetDisSetTimerRunTime()>=0)
    {
        if(timerminute == currentminute)
        {
            if(time.tm_sec < 10)
            {
                return TRUE;
            }else return FALSE;
        }else return FALSE;
    }
    else return FALSE;
}

//判断定时是否关机
uint8_t LookupTimerPwrOff(uint16_t runminute,uint8_t timer_hour,uint8_t timer_minute)
{
    uint16_t offminute=0,currentminute=0;
    struct tm time;

    if(runminute==0)    return FALSE;
    time = gettimebyoffset();
    currentminute = (time.tm_hour*60) + time.tm_min;
    offminute = runminute + timer_hour*60 + timer_minute;

    if(offminute>=1440)  offminute = offminute-1440;

    if(offminute == currentminute)
    {
        if(time.tm_sec < 10)
        {
            return TRUE;
        }else return FALSE;
    }else return FALSE;
}

//判断强热是否关机
uint8_t LookupFastHeatOff(uint16_t startminute)
{
    uint16_t currentminute=0;
    struct tm time;

    time = gettimebyoffset();
    currentminute = (time.tm_hour*60) + time.tm_min;

    if(currentminute<startminute)   currentminute += 1380;
    if(currentminute==(startminute+FASTHEAT_MINUTE_MAX))
    {
        if(time.tm_sec < 10)
        {
            return TRUE;
        }else return FALSE;
    }else return FALSE;
}

//开机
void TurnOn_Device(void)
{
    runState.ctrlCmdFlag = 1;
    runState.disIndex = DISINDEX_PWRON_ANIMATION;    //开机动画显示
    runState.firstPowerOn = 1;
    runState.offingBlinkTime = OFFING_BLINK_TIME;
    runState.temperStandbyState = TEMPER_STADNBY_STATE_NULL;
    runState.standbyOnOffDisState = TEMPER_STADNBY_STATE_NULL;
    runState.bleSetPassword = 0;
}

//故障状态恢复，跳转运行界面
void TurnOn_FaultToRunning(void)
{
    Lcd_Bk_On();

    runState.offingBlinkTime = OFFING_BLINK_TIME;
    runState.runModel = REMOTE_STATE_POWER_ON;
    runState.disIndex = DISINDEX_RUNNING;
    heaterDatas.runningStep = HEATERRUNSTEP_NULL;
    runState.temperStandbyState = TEMPER_STADNBY_STATE_NULL;
    runState.standbyOnOffDisState = TEMPER_STADNBY_STATE_NULL;
    //播放开始加热
//    vEV3P035JPlay(START_HEAT);
    UART1_CMDON();  //发送开机给主机
    runState.bklight_time = GetConfigData_BledTime()*10;
}

//关机带关机动画语音
void TurnOff_Device(void)
{
    runState.ctrlCmdFlag = 1;
    runState.autoOnToOff = FALSE;
    runState.remoteMatchState = DISSETENGMODE_REMOTEMATCH_NULL;
    runState.standbyOnOffDisState = TEMPER_STADNBY_STATE_NULL;
    runState.temperStandbyState = TEMPER_STADNBY_STATE_NULL;
    runState.disSetTimeState = DISSETTIME_STATE_NULL;
    ClearDisSetTimeBlinkCnt();
    runState.bleSetPassword = 0;
    runState.disIndex = DISINDEX_PWROFF_ANIMATION;
    runState.pumPoilFlag = 0;
    runState.coTurnOffFlag = 0;//清除CO关机标志
    vClearScreen();
}

//立即关机，啥都没有
void TurnOff_DeviceNow(void)
{
    runState.autoOnToOff = FALSE;
    runState.remoteMatchState = DISSETENGMODE_REMOTEMATCH_NULL;
    runState.standbyOnOffDisState = TEMPER_STADNBY_STATE_NULL;
    runState.temperStandbyState = TEMPER_STADNBY_STATE_NULL;
    runState.disSetTimeState = DISSETTIME_STATE_NULL;
    ClearDisSetTimeBlinkCnt();
    runState.offingBlinkTime = OFFING_BLINK_TIME;
    runState.disIndex = DISINDEX_PWROFFED;
    runState.runModel = REMOTE_STATE_POWER_OFF;
    runState.pumPoilFlag = 0;
    heaterDatas.runningStep = HEATERRUNSTEP_NULL;
    heaterDatas.faultType = Fault_Null;

    vClearScreen();
    Lcd_Bk_Off();
}

//任务周期200ms
void main_running_task(void)
{
    static uint8_t autoon_cnt=0,autooff_cnt=0,cnt1s=0;
    uint32_t oilpump=0,oilpumpmass=0;
    int16_t temp=0,cabtemp=0;

    if(GetDisSetTimerClockEn()) //定时使能
    {
        if(LookupTimerPwrOn(GetDisSetTimerHour(),GetDisSetTimerMinute()))
        {
            runState.autoOnToOff = TRUE;
            if(GetDisIndex()==DISINDEX_PWROFFED || GetDisIndex()==DISINDEX_PWROFF_ANIMATION || GetDisIndex()==DISINDEX_OFFING)
            {
                TurnOn_Device();
            }
        }
        if(LookupTimerPwrOff(GetDisSetTimerRunTime(),GetDisSetTimerHour(),GetDisSetTimerMinute()) && TRUE == runState.autoOnToOff)
        {
            if(runState.disIndex!=DISINDEX_PWROFFED && runState.disIndex!=DISINDEX_PWROFF_ANIMATION && runState.disIndex!=DISINDEX_OFFING)
            {
                TurnOff_Device();
            }
        }
    }

    //强热模式下运行固定时间关机
    if(GetConfigData_RunMode()==CTRL_RUNMODE_FASTHEAT)
    {
        if(LookupFastHeatOff(runState.fastHeatStartMinute))
        {
            if(runState.disIndex!=DISINDEX_PWROFF_ANIMATION && runState.disIndex!=DISINDEX_OFFING && runState.disIndex!=DISINDEX_PWROFFED)
            {
                TurnOff_Device();
            }
        }
    }

    temp = GetRunningSetTemper();  //设定温度
    cabtemp = GetTemperatureValue()/10;

//    if(runState.mainBoardType != MAIN_BOARD_TYPE_VENTILATE_FASTHART)
    {
        //若开启自动启停  发送:开机、 关机
        if(GetDisSetTemperModeStandby() && GetConfigData_RunMode()==CTRL_RUNMODE_TEMP
            && heaterDatas.faultType == Fault_Null && runState.disIndex != DISINDEX_NULL
            && runState.disIndex != DISINDEX_OFFING && runState.disIndex != DISINDEX_PWROFFED
            && runState.disIndex != DISINDEX_PWROFF_ANIMATION)   //自动启停打开同时还在温控模式下，同时不在关机、关机进行时
        {
            if((temp-cabtemp)>=2)
            {
                //累计30s开机
                autoon_cnt++;
                autooff_cnt=0;
                if(autoon_cnt>AUTO_ONOFF_DELAY_CNT)
                {
                    autoon_cnt = 0;
                    //进入自动启停开机状态
                    runState.temperStandbyState = TEMPER_STADNBY_STATE_ON;
                    if(runState.standbyOnOffDisState != TEMPER_STADNBY_STATE_ON)
                    {
                        if(runState.standbyOnOffDisState == TEMPER_STADNBY_STATE_OFF)
                        {
                            //播放开始加热
                            vEV3P035JPlay(START_HEAT);
                        }
                        runState.ctrlCmdFlag = 1;
                        runState.standbyOnOffDisState = TEMPER_STADNBY_STATE_ON;
                    }
                }
            }else if((temp-cabtemp)<=-2){
                //累计30s关机
                autooff_cnt++;
                autoon_cnt=0;
                if(autooff_cnt>AUTO_ONOFF_DELAY_CNT)
                {
                    autooff_cnt = 0;
                    //进入自动启停关机状态
                    runState.temperStandbyState = TEMPER_STADNBY_STATE_OFF;
                    if(runState.standbyOnOffDisState != TEMPER_STADNBY_STATE_OFF)
                    {
                        runState.standbyOnOffDisState = TEMPER_STADNBY_STATE_OFF;
                        //播放停止加热
                        vEV3P035JPlay(STOP_HEART);
                    }
                }
            }
        }else{
            runState.temperStandbyState = TEMPER_STADNBY_STATE_NULL;
    //        runState.standbyOnOffDisState = TEMPER_STADNBY_STATE_NULL;
            autoon_cnt = 0;
            autooff_cnt = 0;
        }
    }

    //CO超标关机&显示CO超标故障
    if(GetRunningData_CONumber()>=GetConfigData_COWarningData() && GetConfigData_COWarningData()!=0 && runState.runModel!=REMOTE_STATE_POWER_OFF)
    {
        runState.coOverFlag = 1;
        if(GetRunningData_CONumber()>=CO_WARNING_DATA_MAX)
        {
            runState.coTurnOffFlag = 1;
        }
        else
        {
            runState.coTurnOffFlag = 0;
        }
    }
    else
    {
        runState.coOverFlag = 0;
        runState.coTurnOffFlag = 0;
    }

    //4s定时清除co数值
    runState.coClearCnt++;
    if(runState.coClearCnt > 20)
    {
        runState.coClearCnt = 0;
        runState.coNum = 0;
    }

//    if(cnt1s++ >=5)
//    {
//        cnt1s = 0;
//        //剩余油量的计算
//        switch(baseParameters.oilPumpType)
//        {
//        case OILPUMP_TYPE_16UL:
//            oilpump = 16;
//            break;
//        case OILPUMP_TYPE_22UL:
//            oilpump = 22;
//            break;
//        case OILPUMP_TYPE_28UL:
//            oilpump = 28;
//            break;
//        case OILPUMP_TYPE_32UL:
//            oilpump = 32;
//            break;
//        }
//
//        //计算剩余油量
//        if(heaterDatas.runningStep == HEATERRUNSTEP_BURNING)
//        {
//            if(heaterDatas.oilPumpMass>0 && baseParameters.oilCaseVolume!=0)
//            {
//                oilpumpmass = heaterDatas.oilPumpMass/10;
//                if(runState.resOilMass > (oilpump*oilpumpmass))   //燃烧时减剩余油量
//                    runState.resOilMass = runState.resOilMass - (uint32_t)(oilpump*oilpumpmass);
//                else    runState.resOilMass = 0;
//            }
//        }
//    }
}
