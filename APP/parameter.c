/*
 * parameter.c
 *
 *  Created on: 2023年6月24日
 *      Author: wx030
 */
#include "HAL.h"

BaseParameters_Handle_t baseParameters={0}; //基本参数
HeaterData_Handle_t heaterDatas={0};
uint32_t rf_code[4];
//uint8_t mac_code[6];
/*****************************************************************************
 [函数名称]vPARA_INIT()
 [函数功能]参数初始化
 [参    数]
 [返 回 值]
 [备    注]
 *****************************************************************************/
void vPARA_INIT(void)
{
    memset(&baseParameters, 0, sizeof(baseParameters));

    EEPROM_READ(0, &baseParameters, sizeof(baseParameters));
    if (baseParameters.init != 0xAA)
    {
        memset(&baseParameters, 0, sizeof(baseParameters));
        baseParameters.init = 0xAA;
        baseParameters.sttime.tm_year = 2024;
        baseParameters.sttime.tm_mon = 2;
        baseParameters.sttime.tm_mday = 29;
        baseParameters.sttime.tm_hour = 12;
        baseParameters.sttime.tm_min = 30;
        baseParameters.sttime.tm_sec = 0;

        baseParameters.timerClockEn = FALSE;
        baseParameters.timerClockHour = 5;
        baseParameters.timerClockMinute = 30;
        baseParameters.totalHeatTime = 0;
        baseParameters.languageType = 0;

        baseParameters.oilPumpType = OILPUMP_TYPE_22UL;
        baseParameters.runMode = CTRL_RUNMODE_LEVEL;

        baseParameters.oilCaseVolume = 10;      //10L
//        baseParameters.resOilMass = 10000000;   //10L

        baseParameters.temperSensorOffset = 0;
#if GERMAN==0
        baseParameters.languageType = SPEECHLANGUARE_CHINESE;
#elif GERMAN==1
        baseParameters.languageType = SPEECHLANGUARE_ENGLISH;
#elif GERMAN==2
        baseParameters.languageType = SPEECHLANGUARE_ENGLISH;
#endif

        baseParameters.remoteEn = FALSE;
        baseParameters.blePassword = BLEMODULE_DEFAULT_PASSWORD;
        baseParameters.temperModeStandby = FALSE;

        baseParameters.tempUnit = TEMPER_UINT_F;
        baseParameters.bledTime = BLED_TIME_MAX;
        baseParameters.bledDuty = BLED_DUTY_MAX;
        baseParameters.autoTempRemoteTempEn = FALSE;
        baseParameters.bleOnOff = TRUE;
        baseParameters.rf433OnOff = TRUE;
        baseParameters.co433Off = FALSE;
        baseParameters.coWarningData = 300;//CO报警阈值
        vPARA_SAVE();

    }else{
        EEPROM_READ(0, &baseParameters, sizeof(baseParameters));
        EEPROM_READ(128, rf_code, 16);
    }
    runState.setTemper = 30;
    runState.setLevel = 1;
    Time_offset(&baseParameters.sttime);
}

/*****************************************************************************
 [函数名称]vPARA1_SAVE()
 [函数功能]参数保存
 [参    数]
 [返 回 值]
 [备    注]
 *****************************************************************************/
void vPARA_SAVE(void)
{
    baseParameters.init = 0xAA;
    EEPROM_ERASE(0, 256);
    EEPROM_WRITE(0, &baseParameters, sizeof(baseParameters));
    EEPROM_WRITE(128, rf_code, 16);
}

/*****************************************************************************
 [函数名称]GetConfigData_SpeechState(void)
 [函数功能]获取语音类型
 [参    数]
 [返 回 值]语音类型       0 E 1 C 2 R 3 无
 [备    注]
 *****************************************************************************/
uint8_t GetConfigData_SpeechState(void)
{
    return baseParameters.languageType;
}

/*****************************************************************************
 [函数名称]GetHeaterAltitude(void)
 [函数功能]获取海拔高度
 [参    数]
 [返 回 值]海拔高度
 [备    注]
 *****************************************************************************/
uint16_t GetHeaterAltitude(void)
{
    return heaterDatas.altitude;
}

/*****************************************************************************
 [函数名称]GetConfigData_RunMode(void)
 [函数功能]获取当前运行模式
 [参    数]
 [返 回 值]运行模式
 [备    注]
 *****************************************************************************/
uint8_t GetConfigData_RunMode(void)
{
    return baseParameters.runMode;
}

//设置控制器运行模式(温度控制、手动控制、自动启停控制)
void SetConfigData_RunMode(uint8_t mode)
{
    if(mode>=CTRL_RUNMODE_TEMP && mode<CTRL_RUNMODE_MAX)    baseParameters.runMode=mode;
    else baseParameters.runMode = CTRL_RUNMODE_TEMP;
}

/*****************************************************************************
 [函数名称]GetConfigData_remoteEn(void)
 [函数功能]获取当前运行模式
 [参    数]
 [返 回 值]运行模式
 [备    注]
 *****************************************************************************/
uint8_t GetConfigData_remoteEn(void)
{
    return baseParameters.remoteEn;
}

uint8_t GetConfigData_tempUnit(void)
{
    return baseParameters.tempUnit;
}

void SetConfigData_tempUnit(uint8_t unit)
{
    if(unit >= TEMPER_UINT_MAX) baseParameters.tempUnit = TEMPER_UINT_C;
    else                        baseParameters.tempUnit = unit;
}

void SetDisSetTemperUintAdd(void)
{
    baseParameters.tempUnit++;
    if(baseParameters.tempUnit >= TEMPER_UINT_MAX)  baseParameters.tempUnit = TEMPER_UINT_C;
}

void SetDisSetTemperUintDec(void)
{
    baseParameters.tempUnit--;
    if(baseParameters.tempUnit >= TEMPER_UINT_MAX)  baseParameters.tempUnit = TEMPER_UINT_MAX-1;
}

void SwitchDisSetTemperUint(void)
{
    baseParameters.tempUnit = (baseParameters.tempUnit==TEMPER_UINT_C)?TEMPER_UINT_F:TEMPER_UINT_C;
}

uint8_t GetConfigData_BledTime(void)
{
    baseParameters.bledTime = BLED_TIME_MAX;
    return baseParameters.bledTime;
}

void SetConfigData_BledDuty_Add(void)
{
    baseParameters.bledDuty ++;
    if(baseParameters.bledDuty>BLED_DUTY_MAX)   baseParameters.bledDuty = BLED_DUTY_MAX;
}

void SetConfigData_BledDuty_Dec(void)
{
    baseParameters.bledDuty --;
    if(baseParameters.bledDuty>BLED_DUTY_MAX)   baseParameters.bledDuty = BLED_DUTY_MIN;
}

uint8_t GetConfigData_BledDuty(void)
{
    return baseParameters.bledDuty;
}

void SetConfigData_BledDuty(uint8_t duty)
{
    if(duty<=BLED_DUTY_MAX) baseParameters.bledDuty = duty;
    else baseParameters.bledDuty = 1;
}

uint8_t GetHeaterDatas_RunningLevel(void)
{
    return heaterDatas.runningLevel;
}

void SetConfigData_COWarningData_Add(void)
{
    baseParameters.coWarningData += 10;
    if(baseParameters.coWarningData>CO_WARNING_DATA_MAX)   baseParameters.coWarningData = CO_WARNING_DATA_MIN;
}

void SetConfigData_COWarningData_Dec(void)
{
    baseParameters.coWarningData -= 10;
    if(baseParameters.coWarningData>CO_WARNING_DATA_MAX)   baseParameters.coWarningData = CO_WARNING_DATA_MAX;
}

uint16_t GetConfigData_COWarningData(void)
{
    return baseParameters.coWarningData;
}

void SetConfigData_COWarningData(uint16_t data)
{
    baseParameters.coWarningData = data;
}

uint16_t GetRunningData_CONumber(void)
{
    return runState.coNum;
}
