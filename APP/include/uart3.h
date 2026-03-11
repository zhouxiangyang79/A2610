/*
 * uart1.h
 *
 *  Created on: 2025年7月8日
 *      Author: yyz
 */

#ifndef INCLUDE_UART3_H_
#define INCLUDE_UART3_H_

#define WIFI_PROTOCOL_VER   0x00
#define WIFI_PROTOCOL_VER1  0x01

#define SCH_TASK_NUM    1

#pragma pack(1)
typedef struct
{
    uint8_t wifiHead1;          //wifi协议头1
    uint8_t wifiHead2;          //wifi协议头2
    uint8_t wifiProtocolVer;    //wifi协议版本
    uint8_t wifiIndex;          //wifi包序号
    uint16_t wifiPacketLen;     //wifi数据包总长
    uint8_t wifiCmd1;           //wifi命令1
    uint8_t wifiCmd2;           //wifi命令2
    uint8_t wifiData;          //wifi数据包数据部分开始指针
} Wifi_Packet_t;
#pragma pack()

#pragma pack(1)
typedef struct
{
    uint8_t turnOnMode;         //开机运行模式
    uint8_t turnOnParam;        //开机参数
    uint16_t turnOnMinute;      //开机运行时间
} TurnOn_Packet_t;
#pragma pack()

#pragma pack(1)
typedef struct
{
    uint8_t wifiRssi;           //wifi信号强度 0~127
//    uint16_t wifiMAC;           //wifi MAC后4位
    uint8_t wifiConnectState;   //wifi连接状态
    uint8_t wifiWorkState;      //wifi工作状态 0初次 1配网 2登录阿里 3预注册 4OTA
    uint8_t wifiSaveState;      //wifi配网状态 0初次 1未存储 2已存储
    uint8_t wifiClearState;     //wifi清除绑定状态 0已清除 1清除 2还原设置 (10~11)交替刷新
    uint8_t wifiRegState;       //wifi预注册状态 0初次 1未注册 2已注册成功
    uint8_t wifiApState;        //wifi AP模式
    uint8_t wifiOTAProgress;    //wifi OTA进度
} WifiState_Packet_t;
#pragma pack()

#pragma pack(1)
typedef struct
{
    uint16_t wifiV;           //wifi 软件版本号
} WifiVer_Packet_t;
#pragma pack()

//#pragma pack(1)
//typedef struct
//{
//    uint8_t wifiRssi;           //wifi信号强度 0~127
////    uint16_t wifiMAC;           //wifi MAC后4位
//    uint8_t wifiConnectState;   //wifi连接状态
//    uint8_t wifiWorkState;      //wifi工作状态 0初次 1配网 2登录阿里 3预注册 4OTA
//    uint8_t wifiSaveState;      //wifi配网状态 0初次 1未存储 2已存储
//    uint8_t wifiClearState;     //wifi清除绑定状态 0已清除 1清除 2还原设置 (10~11)交替刷新
//    uint8_t wifiRegState;       //wifi预注册状态 0初次 1未注册 2已注册成功
//    uint8_t wifiData1;          //wifi状态后数据1
//} WifiStateData1_Packet_t;
//#pragma pack()

#pragma pack(1)
typedef struct
{
    uint32_t deviceID;          //产品零件号 0x11240905
    uint16_t deviceHW;          //开关硬件版本
    uint16_t deviceSW;          //开关软件版本
    uint16_t languageEn;        //语音包含标志 Bit0英 Bit1中 Bit2俄 Bit3德 Bit4土耳其
    uint8_t bleMac[6];             //蓝牙MAC地址
} GetMACPacket_t;
#pragma pack()

#pragma pack(1)
typedef struct
{
    uint8_t mainBoardType;      //主板类型
    uint8_t meshDevNum;         //Mesh设备数
    uint8_t deviceState;        //开关运行状态
    uint8_t deviceMode;         //开关运行模式
    uint8_t deviceParam;        //开关运行参数
    uint8_t deviceLevel;        //开关计算档位，温控时运行档位有开关计算
    uint8_t mainBoardStep;      //主板运行步骤
    uint8_t dispErrcode;        //显示故障码，不同客户显示故障码需要转换
    uint8_t errCodeType;        //故障码类型
    uint8_t tempUnit;           //温度单位   0℃  1℉   0xFF无此功能
    int16_t deviceTemp;         //开关检测温度
    uint8_t altitudeUnit;       //海拔单位   0m 1ft 0xFF无此功能
    uint16_t altitude;          //海拔高度 *10
    uint16_t supVoltage;        //电源电压 *10
    uint16_t shellTemp;         //壳体温度 *10 与温度单位关联  取到值后按照温度单位转换
    uint16_t coNum;             //CO数值    *10
    uint8_t turnOnType;         //当前开机方式  1-28:对应定时开机  50：按键开机  100：遥控器开机  101：A2422开机  110：平台开机
    uint16_t mainBoardHW;       //主板硬件版本
    uint16_t mainBoardSW;       //主板软件版本
    int8_t tempOffSet;          //温度补偿-15~15℃  -27~27℉
    uint8_t languageType;       //当前语言 0关闭 1英 2中 3俄 4德 5土耳其
    uint8_t oilCaseVolume;      //油箱大小  0~50 步进5  0--               0xFF无此功能
    uint8_t oilPumpType;        //油泵类型       0 16   1 22   2 28   3 32  0xFF无此功能
    uint8_t bkNum;              //背光亮度 0~10  0全关  10最大亮度
    uint8_t tempOnDvi;          //开机温度差(和温度单位关联) 2~10℃/0~18℉
    uint8_t tempOffDvi;         //关机温度差(和温度单位关联) 2~10℃/0~18℉
    uint8_t wifiEn;             //wifi使能
    uint8_t autoOnOff;          //自动启停标志
    uint8_t mainBoardRunMode;   //主板运行模式
    uint16_t runLessTime;       //开机剩余运行时间
} BaseUartPacket_t;
#pragma pack()

#pragma pack(1)
typedef struct
{
    uint8_t mainBoardRunLevel;  //主板运行档位
    uint32_t mainBoardRunTime;  //主板运行时间
    uint8_t  ignitionVoltage;   //点火电压 *10
    uint8_t  ignitionPower;     //点火功率
    uint8_t  ignitionCurrent;   //点火电流 *10
    uint8_t  ignitionRes;       //点火电阻
    uint8_t  windCurrent;       //风机电流 *10
    uint16_t windSpeed;         //风机转速
    int16_t  exhaustTemp;       //尾气温度 *10
    int16_t  inputAirTemp;      //进风温度 *10
    int16_t  outputAirTemp;     //出风温度 *10
    uint16_t oilPumpMass;       //泵油频率 *10
} MainBoard_DebugPacket_t;
#pragma pack()

#pragma pack(1)
typedef struct
{
    uint8_t schTaskEn; //定时使能
    uint8_t schTaskWeek;//定时星期
    uint8_t schTaskStartHour;//定时开始小时
    uint8_t schTaskStartMinute;//定时开始分钟
    uint16_t schTaskRunMinute;//定时运行分钟
    uint8_t schTaskRunMode;//定时运行分钟
    uint8_t schTaskRunParam;//定时运行参数
} Sch_Task_t;
#pragma pack()
#pragma pack(1)
typedef struct
{
    uint8_t schTaskNum; //定时总数
    Sch_Task_t schTaskBuf[SCH_TASK_NUM];//定时任务
} Sch_TaskPacket_t;
#pragma pack()

#pragma pack(1)
typedef struct
{
    uint8_t paramAltitudeUnit;  //海拔单位
    uint8_t paramTTempUnit;     //温度单位
    uint16_t paramDeviceTime;   //设备时间
    int8_t paramTempOffset;     //温度偏差
    uint8_t paramLanguageType;  //当前语音
    uint8_t paramOilCaseVolume; //油箱体积
    uint8_t paramOilPumpType;   //油泵类型
    uint8_t paramBledLevel;     //背光亮度
    uint8_t paramTempOnDvi;     //开机温度差
    uint8_t paramTempOffDvi;    //关机温度差
    uint8_t paramWifiEn;        //wifi使能
    uint8_t autoOnOff;          //自动启停标志
} Param_SetPacket_t;
#pragma pack()

typedef enum
{
    WIFI_CONNECT_NULL = 0,//无配网信息
    WIFI_CONNECT_ING = 1,//正在联网
    WIFI_CONNECT_OK = 2,//wifi连接成功
    WIFI_CONNECT_ERR = 3,//wifi连接失败
    WIFI_CLEAR_OK = 20, //wifi清除绑定成功
}WIFI_CONNECT_STATE;

typedef enum
{
    WIFI_WORK_NULL = 0,//初次
    WIFI_WORK_CONN = 1,//配网
    WIFI_WORK_THING = 2,//连接阿里
    WIFI_WORK_REG = 3,//预注册
    WIFI_WORK_OTA = 4,//OTA
}WIFI_WORK_STATE;

typedef enum
{
    WIFI_SET_DISINDEX_CONNECT=0,//wifi连接
    WIFI_SET_DISINDEX_OTA,      //wifiOTA
    WIFI_SET_DISINDEX_CLEAR,    //wifi清除绑定
    WIFI_SET_DISINDEX_REG,      //wifi预注册
    WIFI_SET_DISINDEX_STATE,    //wifi状态
    WIFI_SET_DISINDEX_MAX
}WIFI_SET_DISINDEX;

typedef enum
{
    WIFI_CONTROL_MODE_NULL = 0,
    WIFI_CONTROL_MODE_LEVEL = 1,
    WIFI_CONTROL_MODE_TEMP = 2,
    WIFI_CONTROL_MODE_VENTILATE = 3,
    WIFI_CONTROL_MODE_FASTHEAT = 4,
    WIFI_CONTROL_MODE_AUTO = 6,
    WIFI_CONTROL_MODE_MAX,
}WIFI_CONTROL_MODE;

typedef enum
{
    WIFI_CMD1_DEVICE_STATE = 0x00,
    WIFI_CMD2_DEVICE_STATE_NORMAL_PKT = 0x00,//常用包
    WIFI_CMD2_DEVICE_STATE_DEBUG_PKT = 0x01,//诊断包
    WIFI_CMD2_DEVICE_STATE_ERR_PKT = 0x02,//故障包
    WIFI_CMD2_DEVICE_STATE_MAC_PKT = 0x03,//获取MAC包
    WIFI_CMD1_DEVICE_CONTROL = 0x01,
    WIFI_CMD2_DEVICE_CONTROL_OFF = 0x00,//关机
    WIFI_CMD2_DEVICE_CONTROL_ON = 0x01,//开机
    WIFI_CMD2_DEVICE_CONTROL_UPLOAD = 0x02,//设备端开关状态变化上报
    WIFI_CMD1_DEVICE_SCH_TASK = 0x02,
    WIFI_CMD2_DEVICE_SCH_TASK_QUERY = 0x00,//定时查询
    WIFI_CMD2_DEVICE_SCH_TASK_ADD = 0x01,//定时新增
    WIFI_CMD2_DEVICE_SCH_TASK_EDIT = 0x02,//定时修改
    WIFI_CMD2_DEVICE_SCH_TASK_DEL = 0x03,//定时删除
    WIFI_CMD2_DEVICE_SCH_TASK_UPLOAD = 0x04,//设备端定时变化上报
    WIFI_CMD1_DEVICE_PARAM = 0x03,
    WIFI_CMD2_DEVICE_PARAM_QUERY = 0x00,//属性查询
    WIFI_CMD2_DEVICE_PARAM_SET = 0x01,//属性设置
    WIFI_CMD2_DEVICE_PARAM_UPLOAD = 0x02,//设备端属性变化上报
    WIFI_CMD2_DEVICE_PARAM_REST = 0x03,//设备端属性恢复出厂
    WIFI_CMD1_WIFI_CONTROL = 0x04,
    WIFI_CMD2_WIFI_CONTROL_QUERY = 0x00,//wifi状态查询
    WIFI_CMD2_WIFI_CONTROL_CONNECT = 0x01,//wifi配网
    WIFI_CMD2_WIFI_CONTROL_CLEAR = 0x02,//wifi清除绑定
    WIFI_CMD2_WIFI_CONTROL_REGISTER = 0x03,//wifi预注册
    WIFI_CMD2_WIFI_CONTROL_OTA = 0x04,//wifi OTA
    WIFI_CMD2_WIFI_CONTROL_GETVER = 0x05,//获取wifi版本号
    WIFI_CMD1_MESH_CONTROL = 0x05,  //MESH操作
    WIFI_CMD1_BLE_CONTROL = 0x06,//蓝牙操作
    WIFI_CMD2_HANDSHAKE = 0x00, //蓝牙握手
}WIFI_CONTROL_CMD_STATE;

typedef enum
{
    WIFI_SENDPKT_STATE_NULL=0,
    WIFI_SENDPKT_STATE_HEARTBEAT,   //心跳包、常用数据
    WIFI_SENDPKT_STATE_DIAGNOSIS,   //诊断数据包
    WIFI_SENDPKT_STATE_DEVICE_ERR,  //设备故障数据包
    WIFI_SENDPKT_STATE_DEVICE_MAC_RESPONSE,  //获取设备MAC应答
    WIFI_SENDPKT_STATE_ATTRIBUTE_GET_RESPONSE,//属性查询应答
    WIFI_SENDPKT_STATE_ATTRIBUTE_SET_RESPONSE,//属性设置应答
    WIFI_SENDPKT_STATE_ATTRIBUTE_CHANGE_RESPONSE,//属性修改上报
    WIFI_SENDPKT_STATE_ATTRIBUTE_RESET_RESPONSE,//属性恢复出厂应答
    WIFI_SENDPKT_STATE_TURN_OFF_RESPONSE,//关机应答,wifi
    WIFI_SENDPKT_STATE_TURN_ON_RESPONSE,//开机应答,wifi
    WIFI_SENDPKT_STATE_TURN_ONOFF_RESPONSE,//开关机上报 定时、433遥控、A2422
    WIFI_SENDPKT_STATE_SCHTASK_QUERY_RESPONSE,//定时查询上报
    WIFI_SENDPKT_STATE_SCHTASK_ADD_RESPONSE,//定时新增上报
    WIFI_SENDPKT_STATE_SCHTASK_EDIT_RESPONSE,//定时修改上报
    WIFI_SENDPKT_STATE_SCHTASK_DEL_RESPONSE,//定时删除上报
    WIFI_SENDPKT_STATE_SCHTASK_CHANGE_RESPONSE,//定时变化主动上报
    WIFI_SENDPKT_WIFI_CONTROL_GET_STATE,//查询wifi状态
    WIFI_SENDPKT_WIFI_CONTROL_CONNECT,//wifi配网
    WIFI_SENDPKT_WIFI_CONTROL_CLEAR,//wifi清除绑定
    WIFI_SENDPKT_WIFI_CONTROL_REG,//wifi预注册
    WIFI_SENDPKT_WIFI_CONTROL_OTA,//wifi OTA
    WIFI_SENDPKT_WIFI_CONTROL_GETVER,//获取wifi软件版本

    BLE_SENDPKT_BLE_HANDSHAKE,//蓝牙握手
    WIFI_SENDPKT_STATE_MAX
}WIFI_SENDPKT_STATE;

void UART3_INIT(void);
void wifiState_Get(uint8_t worktstate,uint8_t cmd2,uint8_t state);
uint8_t Make_GetMACPacket(uint8_t* buf);
uint16_t Make_NormalPacket(uint8_t* buf);
uint16_t Make_DebugPacket(uint8_t* buf);
uint16_t Make_SchTaskPacket(uint8_t* buf);
uint16_t Make_WifiControlPacket(uint8_t* buf);
void Uart3_SendPacket(uint8_t pkt_type);
int16_t TempFtoC(int16_t temp);
int16_t TempCtoF(int16_t temp);
int16_t TempMul18(int16_t temp);
int16_t TempDiv18(int16_t temp);
int16_t TempChange(int16_t temp, uint8_t tunit);
uint16_t AltitudeChange(uint16_t al,uint8_t aunit);

//void UART3_SendPacket(uint16_t len,uint8_t *buf);

#endif /* INCLUDE_UART1_H_ */
