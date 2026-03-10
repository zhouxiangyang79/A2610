/*
 * parameter.h
 *
 *  Created on: 2023年6月24日
 *      Author: wx030
 */

#ifndef PARAMETER_H_
#define PARAMETER_H_
#include "date_time.h"
#include <time.h>

//基础参数
typedef struct
{
    uint8_t init;               //初始化标志 0xAA
    struct tm sttime;           //当前设备时间
    int32_t timeoffset;         //时间校准偏移
    uint8_t  runMode;           //运行模式
    uint8_t  timerClockEn;      //定时开机使能    0关      1开
    uint8_t  timerClockHour;    //定时小时
    uint8_t  timerClockMinute;  //定时分钟
    uint16_t totalHeatTime;     //定时开机运行时间
    uint8_t  languageType;      //语音类型      0无  1E英语 2G德语 3R俄语


    uint16_t runTime;           //定时开机运行时间  30分钟步进最大10小时
    uint8_t oilPumpType;        //油泵类型       0 16   1 22   2 28   3 32
    uint8_t  oilCaseVolume;     //油箱大小  --~50 步进5

    int8_t temperSensorOffset;  //温度偏差值
    uint8_t  remoteEn;          //遥控使能
//    uint8_t  bleEn;           //蓝牙使能
    uint16_t blePassword;       //蓝牙密码
    uint8_t  temperModeStandby; //自动温度自动待机开关
//    uint8_t  coLimit;
    uint8_t tempUnit;           //温度单位选择
    uint8_t bledTime;           //背光打开时间  0常亮
    uint8_t bledDuty;           //背光占空比
    uint8_t autoTempRemoteTempEn;//至优主机温度模式是否使能主机参考手柄检测温度
    uint8_t bleOnOff;           //蓝牙开关
    uint8_t rf433OnOff;         //433遥控开关
    uint8_t co433Off;           //433接收CO超标关机使能

    uint16_t coWarningData;      //CO报警阈值
//    uint8_t isAuto;             //自动启停使能  0关      1开
//    uint8_t bluetoothswitch;
//    uint8_t remotecontrolmatching;
//    uint8_t automaticheating;
} BaseParameters_Handle_t;

//系统参数
typedef struct
{
    uint8_t runningMode;        //运行模式 01自动 02手动 03泵油
    uint8_t runningStep;        //运行步骤（主板读取）  00关机 01自检 02启动/点火 03燃烧 04停机散热
    uint8_t runningLevel;       //运行档位
    uint8_t faultType;          //故障码
    uint16_t version;           //设备版本号
    uint16_t supplyVoltage;     //供电电压
    uint16_t altitude;           //海拔高度

    int16_t shellTemp;          //壳体温度    x10
    uint32_t runTime;           //运行时间
    uint16_t oilPumpMass;       //油泵频率   x10
    uint8_t  ignitionVoltage;   //点火电压
    uint8_t  ignitionPower;     //点火功率
    uint8_t  ignitionCurrent;   //点火电流
    uint8_t  ignitionRes;       //点火电阻
    uint16_t windCurrent;       //风机当前转速

    int16_t cabTemp;            //驾驶室温度
    int16_t cabAltitude;       //开关检测海拔高度
    uint8_t sensorType;         //传感器类型 0:null  1:CO传感器
//    uint16_t oilHz;             //供油量    //加热器运行档位
} HeaterData_Handle_t;

#define  DISP_BLINK_FREQUENCY   10

#define CO_OFF_CODE 0x00F7A5FD
#define BLEMODULE_DEFAULT_PASSWORD      0x0C22  //1234   12->0x0C   34->0x22

#define TEMPER_UINT_C                   0   //温度摄氏度
#define TEMPER_UINT_F                   1   //温度华氏度
#define TEMPER_UINT_MAX                 2

#define BLED_TIME_MAX                   120  //背光最大时间120s
#define BLED_DUTY_MAX                   8  //背光最大占空比100
#define BLED_DUTY_MIN                   0

#define CO_WARNING_DATA_MAX             500
#define CO_WARNING_DATA_MIN             0

#define CTRL_RUNMODE_TEMP               0   //温度控制
#define CTRL_RUNMODE_LEVEL              1   //手动控制
//#define CTRL_RUNMODE_AUTO               2   //自动启停模式
#define CTRL_RUNMODE_VENTILATE          2   //通风
#define CTRL_RUNMODE_FASTHEAT           3   //强热
#define CTRL_RUNMODE_MAX                4

#define CTRL_RUNMODE_TEMP_MAX           36
#define CTRL_RUNMODE_TEMP_MIN           8
#define CTRL_RUNMODE_HTEMP_MAX          97
#define CTRL_RUNMODE_HTEMP_MIN          46

#define POWER_LEVEL_NUM_MAX             10
#define POWER_LEVEL_NUM_MIN             1

//#define BK_LIGHT_CNT                    200     //200*100ms
#define VOICE_INTERVAL_CNT              50      //语音播放间隔   4s
#define AUTO_ONOFF_DELAY_CNT            150     //自动启停判断时间30s间隔
#define MANUPUMP_TIMESPACE              3000    //泵油时间 300s   3000*100ms
#define OFFING_BLINK_TIME               25      //2.5s
#define FASTHEAT_MINUTE_MAX             120     //强热最大运行时间 120分钟

#define SERIAL_GET_DATA                 0
#define SERIAL_SET_CMD                  1
#define SERIAL_SEND_TIME                10      //串口发送数据间隔

//遥控器状态
#define REMOTE_STATE_POWER_OFF          0       //关机
#define REMOTE_STATE_POWER_ON           1       //开机
#define REMOTE_STATE_PUMPOIL            2       //泵油

#define TEMPER_STADNBY_STATE_NULL       0       //null
#define TEMPER_STADNBY_STATE_ON         1       //自动启停开机
#define TEMPER_STADNBY_STATE_OFF        2       //自动启停关机

//主机故障码
#define Fault_Null                              0  //无故障
#define Fault_Start                             1  //启动失败
#define Fault_Flameout                          2  //缺油熄火
#define Fault_Power                             3  //电压异常
#define Fault_OutAirTemp_Senser                 4  //出风温度传感器异常
#define Fault_IntAirTemper_Senser               5  //进风温度传感器异常
#define Fault_OilPump                           6  //油泵断路
#define Fault_WindMotor                         7  //风机异常
#define Fault_Ignition                          8  //预热塞异常
#define Fault_ShellTemper_Protect               9  //高温保护
#define Fault_ShellTemper_Protect_Senser        10 //高温传感器异常
#define Fault_Contrlor_Error                    11 //控制盒故障
#define Fault_NoSecretKey                       16 //密钥不对
#define Fault_CoPpmHigh                         20 //
#define Fault_LOWBATTERY                        50 //20250605  新增电池电量低报警

/*********************控制加热器**********************/
#define  HEATER_RUNMODE_NULL                    0x00
#define  HEATER_RUNMODE_AUTO                    0x01
#define  HEATER_RUNMODE_MANU                    0x02
#define  HEATER_RUNMODE_PUMPOIL                 0x03
#define  HEATER_RUNMODE_STANDBY                 0x04
#define  HEATER_RUNMODE_VENTILATE               20

//主机运行步骤
#define  HEATERRUNSTEP_NULL                     0x00
#define  HEATERRUNSTEP_SELFCHECK                0x01    //自检
#define  HEATERRUNSTEP_STARTIGNITE              0x02    //点火
#define  HEATERRUNSTEP_BURNING                  0x03    //燃烧
#define  HEATERRUNSTEP_STOP                     0x04    //停机
#define  HEATERRUNSTEP_REST                     0x05    //暂停
#define  HEATERRUNSTEP_VENTILATE                0x06    //通风

//-------------------扩展状态-------------------------
//#define  HEATERSTATE_NULL                       0
//#define  HEATERSTATE_PWR_OFF                    1
//#define  HEATERSTATE_PWR_OFFEING_SUCC           2 //SBUS
////----------------------------------------------------
//#define  HEATERSTATE_PUMPOIL                    3 //SBUS
//#define  HEATERSTATE_STANDBY                    4 //SBUS
//#define  HEATERSTATE_VENTILATE                  20//泵油
//#define  HEATERSTATE_PWROFFING                  5 //SBUS
//#define  HEATERSTATE_PWRON                      6 //SBUS
/*****************************************************/
//-------------------扩展状态-------------------------
#define  HEATERSTATE_WAIT_DATA                  7
#define  HEATERSTATE_RUNNING                    8
#define  HEATERSTATE_FAULT                      9
#define  HEATERSTATE_PWR_OFFED                  10

#define  HEATERSTATE_PP_OIL_SUCC                12
#define  HEATERSTATE_STANDBY_SUCC               13
//----------------------------------------------------
#define  HEATERSTATE_STANDBY_WAITON             14

#define  HEATER_AUXSTATE_NULL                   0x00
#define  HEATER_AUXSTATE_PWROFF                 0x01

#define  HEATER_PWRON_TIMEOUTSPACE              30
#define  HEATER_PWROFF_TIMEOUTSPACE             600

#define  DIS_ALTITUDE_ENABLE                    3000

#define  FAULT_TIMEOUTSPACE                     1800

#define  OILCASE_VOLUME_MAX                     50

#define  TEMPERMODE_STATDBYOFF_DELAYTIMESPACE   30
#define  TEMPERMODE_STATDBYON_DELAYTIMESPACE    30
#define  ALTITUDE_DIS                           3000

typedef enum
{
    MAIN_BOARD_TYPE_NONE_VENTILATE=1,           //老主板无通风、强热(老3线)
    MAIN_BOARD_TYPE_5LINE_NONE_VENTILATE,       //5线无通风
    MAIN_BOARD_TYPE_5LINE_VENTILATE,            //5线有通风
    MAIN_BOARD_TYPE_VENTILATE,                  //致系列带通风、无强热模式
    MAIN_BOARD_TYPE_VENTILATE_FASTHART,         //致系列带通风、强热模式
    MAIN_BOARD_TYPE_MAX,
    MAIN_BOARD_TYPE_CHICKTIM_MAX=50,            //低版本主机切换状态最大时间
}MAIN_BOARD_TYPE;

typedef enum
{
    BLE_CMD_GETDATA=1,
    BLE_CMD_SET_MODEL,
    BLE_CMD_TURNONOFF,
    BLE_CMD_CHANGEVALUE,
//
//    BLE_CMD_TURNOFF,
//    BLE_CMD_SET_LEVEL,
//    BLE_CMD_SET_TEMPER,
//
//    BLE_CMD_PUMP_OIL,

    BLE_CMD_SET_TIME=10,
    BLE_CMD_SET_TIMER,
    BLE_CMD_SET_TIMER_RUNTIME,
    BLE_CMD_SET_TIMER_EN,
    BLE_CMD_SET_LANGUAGE,
    BLE_CMD_SET_TEMPER_UINT,
    BLE_CMD_SET_OILCASL_VOLUME,
    BLE_CMD_SET_OILPUMP_TYPE,
    BLE_CMD_SET_AUTO_ONOFF_SW,
    BLE_CMD_SET_SET_ALTITUDE_UINT,
    BLE_CMD_SET_TEMPER_OFFSET,
    BLE_CMD_SET_BLED_TIME,
}BEL_CMD;

typedef enum
{
    DISSETTEMPER_STATE_NULL=0,
    DISSETTEMPER_STATE_BLINKSET,
    DISSETTEMPER_STATE_BLINKOFF,
    DISSETTEMPER_STATE_BLINKINT,
    DISSETTEMPER_STATE_BLINKSET_TIME=35,
} DISSETTEMPER_STATE;

typedef enum
{
    DISSETTIME_STATE_NULL=0,
    DISSETTIME_STATE_HOUR,
    DISSETTIME_STATE_MINUTE,
    DISSETTIME_STATE_HOUR_MINUTE,
    DISSETTIME_STATE_MAX,
} DISSETTIME_STATE;

typedef enum
{
    SPEECHLANGUARE_OFF = 0,
    SPEECHLANGUARE_ENGLISH,
    SPEECHLANGUARE_CHINESE,     //SPEECHLANGUARE_GE德语
    SPEECHLANGUARE_RUSSIAN,
    SPEECHLANGUARE_MAX,
} SPEECHLANGUARE_TYPE;

typedef enum
{
    DISSETENGMODE_REMOTEMATCH_NULL=0,
    DISSETENGMODE_REMOTEMATCH_KEYADD,   //遥控:+
    DISSETENGMODE_REMOTEMATCH_KEYDEC,   //遥控:-
    DISSETENGMODE_REMOTEMATCH_KEYMODE,  //遥控:模式
    DISSETENGMODE_REMOTEMATCH_KEYPOWER, //遥控:电源
    DISSETENGMODE_REMOTEMATCH_CLEARSUC, //遥控清除完成
} DISSETENGMODE_REMOTEMATCH;

//typedef enum
//{
//    DISSETENGMODE_REMOTEMATCH_KEYADD=0,   //遥控:+
//    DISSETENGMODE_REMOTEMATCH_KEYDEC,   //遥控:-
//    DISSETENGMODE_REMOTEMATCH_KEYON,  //遥控:开机/模式切换
//    DISSETENGMODE_REMOTEMATCH_KEYOFF, //遥控:关机
//} DISSETENGMODE_REMOTEMATCH;

#define TOTAL_OILMASSLEVEL                      5
typedef enum
{
    OILPUMP_TYPE_NULL=0,
    OILPUMP_TYPE_16UL,
    OILPUMP_TYPE_22UL,
    OILPUMP_TYPE_28UL,
    OILPUMP_TYPE_32UL,
}OILPUMP_TYPE;


typedef enum
{
    RUNSTATE_NULL=0,
    RUNSTATE_GOSLEEP,
    RUNSTATE_SLEEP,
    RUNSTATE_WAKEUP,
    RUNSTATE_HEATEROFFED,
    RUNSTATE_WAITHEATEROFFED,
    RUNSTATE_PWROFF,
    RUNSTATE_PWRON,

    RUNSTATE_WAITHEATER_ONLINE,
    RUNSTATE_PWRONFAILD,
    RUNSTATE_RUNNING,
    RUNSTATE_PWROFFFAILD,
    RUNSTATE_FAULT,

    RUNSTATE_GOTOMANUALPUMP,
    RUNSTATE_ENGMODE,
    RUNSTATE_REMOTEMTH,
    RUNSTATE_BLEMTH,
    RUNSTATE_SETUPCFGHEATER,
    RUNSTATE_SETUPCFGCTRL,
    RUNSTATE_GOTOVENTILATE,
    //RUNSTATE_STANDBY
} RUNSTATE;

void vPARA_INIT(void);
void vPARA_SAVE(void);
uint8_t GetConfigData_SpeechState(void);
uint16_t GetHeaterAltitude(void);
uint8_t GetConfigData_RunMode(void);
void SetConfigData_RunMode(uint8_t mode);
uint8_t GetConfigData_remoteEn(void);
uint8_t GetConfigData_tempUnit(void);
void SetConfigData_tempUnit(uint8_t unit);
void SetDisSetTemperUintAdd(void);
void SetDisSetTemperUintDec(void);
void SwitchDisSetTemperUint(void);
uint8_t GetConfigData_BledTime(void);
void SetConfigData_BledDuty_Add(void);
void SetConfigData_BledDuty_Dec(void);
uint8_t GetConfigData_BledDuty(void);
void SetConfigData_BledDuty(uint8_t duty);
uint8_t GetHeaterDatas_RunningLevel(void);
void SetConfigData_COWarningData_Add(void);
void SetConfigData_COWarningData_Dec(void);
uint16_t GetConfigData_COWarningData(void);
void SetConfigData_COWarningData(uint16_t data);
uint16_t GetRunningData_CONumber(void);

extern BaseParameters_Handle_t baseParameters;
extern HeaterData_Handle_t heaterDatas;
#endif /* PARAMETER_H_ */
