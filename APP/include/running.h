/*
 * parameter.h
 *
 *  Created on: 2023年6月24日
 *      Author: wx030
 */

#ifndef _RUNNING_H_
#define _RUNNING_H_

//运行时参数
typedef struct
{
    int16_t setTemper;          //设定温度
    int16_t setHTemper;         //设定华氏温度
    uint8_t auto_setLevel;      //温控模式时档位
    uint8_t auto_Level;         //由于计算档位，温度设定不闪烁后将值传给auto_setLevel
    uint8_t setLevel;           //手动模式时档位
    uint8_t ventilateLevel;     //通风档位
    uint8_t runModel;           //遥控器存储的运行模式，与主机运行模式比较。状态不对应需一直发送控制指令
    uint8_t firstPowerOn;       //上电标志  0刚上电   1开过机
    uint8_t temperStandbyState; //自动启停模式状态 0null  1自动启停开机  2自动启停关机
    uint32_t resOilMass;        //uL  剩余油量
    uint8_t bleSetPassword;     //设置蓝牙密码标志

    uint8_t disIndex;
    uint8_t disSetTemp;            //运行时修改设定温度置1开始闪烁  5s后误操作退出闪烁显示驾驶室温度
    uint8_t disSetTempBlinkCnt;
    uint8_t disSetTimeState;        //0不设置  1设置小时  2设置分钟  3小时分钟同时设置
    uint8_t disSetTimeBlinkCnt;
    uint16_t disLedIndex;
    uint8_t remoteMatchState;       //添加遥控时状态
//    uint8_t usart1Send;           //1处于发送状态   0空闲状态
    uint8_t usart1DelayCnt;         //串口获取数据延时
    uint16_t bklight_time;          //背光关闭延时
    uint16_t voiceIntervalCnt;      //语音播放间隔
    uint8_t offingBlinkTime;        //关机时off闪烁时间
    uint8_t autoOnToOff;            //自动开机后需要自动关机标志  0不需要自动关机    1需要自动关机
    uint8_t standbyOnOffDisState;   //开机时温控自动启停模式    0null    1自动启停开机动画      2自动启停关机动画
    uint8_t standbyOffDisCnt;       //自动启停关机时，修改温度Off不显示计数
    uint8_t mainBoardType;          //主板类型  0老主板(无通风、强热)    1(有通风、强热)

    uint8_t ctrlCmdFlag;            //串口发送控制命令标志
    uint8_t pumPoilFlag;            //针对至优泵油时，发现主板已经进入泵油了。以后就不发泵油指令了
    uint8_t bledDuty;               //运行时LED亮度
    uint16_t fastHeatStartMinute;   //强热起始时间、分钟

    uint8_t coBeepFlag;             //CO超标声音报警标志
    uint8_t coOverFlag;             //CO超标标志
    uint8_t coTurnOffFlag;          //CO超标关机标志
    uint8_t coOffset;               //CO偏移量
    uint8_t coAccuracy;             //CO精度
    uint16_t coData;                //CO检测值
    uint16_t coNum;                 //CO物理数值
    uint8_t coClearCnt;             //清除串口co数值计数
    uint8_t coDisTime;              //E11是CO数值显示时间变量


    uint16_t vccAdcMax;             //电源Vcc最大ad值
    uint16_t vccAdcMin;             //电源Vcc最小ad值
    uint16_t vccAdcDiffer;          //最大与最小差
} Run_Handle_t;

void runState_Init(void);
void clear_State(void);
uint8_t GetDisIndex(void);
uint8_t GetRunningSetLevel(void);
uint8_t GetRunningAutoSetLevel(void);
void SetRunningSetLevel(uint8 level);
uint8_t GetRunningAutoLevel(void);
void SetRunningAutoLevel(uint8 level);
void SetRunningAutoSetLevel(uint8 level);
void RunningSetLevel_Add(void);
void RunningSetLevel_Dec(void);
void ControlModel_Change(void);
int16_t GetRunningSetTemper(void);
int16_t GetRunningSetHTemper(void);
void SetRunningSetTemper(uint16 temp);
int16_t GetTemperatureValue(void);
uint8_t GetDisSetTemperBlink(void);
void SetDisSetTemperBlink(uint8_t state);
void CleardisSetTempBlinkCnt(void);
void RunningSetTemper_Add(void);
void RunningSetTemper_Dec(void);
void RunningVentilateLevel_Add(void);
void RunningVentilateLevel_Dec(void);
void SetRunningVentilateLevel(uint8_t level);
uint8_t GetRunningVentilateLevel(void);
uint8_t GetDisSetTimeState(void);
void ClearDisSetTimeBlinkCnt(void);
void SetDisSetTimeState(void);
void SetDisRunState_disIndexAdd(void);
void SetDisRunState_disIndexDec(void);
void SetDisEngModeAdd(void);
void SetDisEngModeDec(void);
void SetDisSetTimeAdd(void);
void SetDisSetTimeDec(void);
void SetDisSetTimeHourAdd(void);
void SetDisSetTimeHourDec(void);
void SetDisSetTimeMinuteAdd(void);
void SetDisSetTimeMinuteDec(void);
void SetDisSetTime(uint8 hour,uint8 minute);
void SetDisSetTimerHourAdd(void);
void SetDisSetTimerHourDec(void);
uint8_t GetDisSetTimerHour(void);
void SetDisSetTimerMinuteAdd(void);
void SetDisSetTimerMinuteDec(void);
uint8_t GetDisSetTimerMinute(void);
void SetDisSetTimer(uint8 hour,uint8 minute);
void SetDisSetTimerRunTimeAdd30(void);
void SetDisSetTimerRunTimeDec30(void);
void SetDisSetTimerRunTime(uint16_t minute);
uint16_t GetDisSetTimerRunTime(void);
void SetDisSetTimerClockEn(uint8 en);
uint8_t GetDisSetTimerClockEn(void);
uint8_t GetDisSetLanguageType(void);
void SetDisSetLanguageType(uint8_t type);
void SetDisSetLanguageTypeAdd(void);
void SetDisSetLanguageTypeDec(void);
int8_t GetDisSetTemperOffSet(void);
void SetDisSetTemperOffSet(int8_t offset);
void SetDisSetTemperOffSetAdd(void);
void SetDisSetTemperOffSetDec(void);
uint8_t GetDisSetOilCaseVolume(void);
void SetDisSetOilCaseVolume(uint8_t dat);
void SetDisSetOilCaseVolumeAdd5(void);
void SetDisSetOilCaseVolumeDec5(void);
uint8_t GetDisSetOilPumpType(void);
void SetDisSetOilPumpType(uint8_t type);
void SetDisSetOilPumpTypeAdd(void);
void SetDisSetOilPumpTypeDec(void);
void SetDisSetBleOnOff(uint8_t state);
uint8_t GetDisSetBleOnOff(void);
void SetDisSet433OnOff(uint8_t state);
uint8_t GetDisSet433OnOff(void);
uint8_t GetDisSetTemperModeStandby(void);
void SetDisSetTemperModeStandby(uint8_t state);
void ClearDisSetRemoteMatchState(void);
void TurnOn_Device(void);
void TurnOn_FaultToRunning(void);
void TurnOff_Device(void);
void TurnOff_DeviceNow(void);
void main_running_task(void);

extern Run_Handle_t runState;

#endif /* _RUNNING_H_ */
