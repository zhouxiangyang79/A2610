/*
 * uart3.c
 *
 *  Created on: 2025年7月8日
 *      Author: yyz
 */
#include "HAL.h"
#include "parameter.h"
#include "running.h"
#include "uart3.h"
#include "app_mesh_config.h"
#include "lcd.h"
#include "wifi.h"
//
uint8_t Rx3Buff[512];
uint16_t buf3_size = 0;

void UART3_INIT(void)
{
	/* 配置串口1：先配置IO口模式，再配置串口 */
	GPIOA_SetBits(bTXD3);
	GPIOA_SetBits(bRXD3); //串口0接收
	GPIOA_ModeCfg(bRXD3, GPIO_ModeIN_PU);
	GPIOA_ModeCfg(bRXD3, GPIO_ModeIN_PU); // RXD-配置上拉输入
	GPIOA_ModeCfg(bTXD3, GPIO_ModeOut_PP_20mA); // TXD-配置推挽输出，注意先让IO口输出高电平
	UART3_DefInit();
	UART3_BaudRateCfg(115200);
	UART3_ByteTrigCfg(UART_7BYTE_TRIG);
	UART3_INTCfg(ENABLE, RB_IER_RECV_RDY | RB_IER_LINE_STAT);
	PFIC_EnableIRQ(UART3_IRQn);
}

//void Uart3_DataProcess(uint8_t data)
//{
//    Rx3Buff[buf3_size++] = data;
//}

void wifiState_Get(uint8_t worktstate,uint8_t cmd2,uint8_t state)
{
    switch(worktstate)
    {
    case WIFI_WORK_NULL:
        break;
    case WIFI_WORK_CONN://配网
        if(state == 8)                  runState.wifiConnectState = WIFI_CONNECT_OK;
        else if(state>=1 && state<=4)   runState.wifiConnectState = WIFI_CONNECT_ING;
        else if(state == 6)             runState.wifiConnectState = WIFI_CONNECT_ING;
        else if(state==5 || state==7)   runState.wifiConnectState = WIFI_CONNECT_ERR;
        break;
    case WIFI_WORK_THING://阿里
        switch(state)
        {
        case 0://wifi正在连接
            if(cmd2 == WIFI_CMD2_WIFI_CONTROL_CONNECT)  runState.wifiConnectState = WIFI_CONNECT_ING;
            else                                        runState.wifiConnectState = WIFI_CONNECT_NULL;
            break;
        case 1://wifi初始化
            break;
        case 2://wifi正在连接
            runState.wifiConnectState = WIFI_CONNECT_ING;
            break;
        case 3://wifi连接成功
            runState.wifiConnectState = WIFI_CONNECT_OK;
            break;
        case 4://wifi连接失败
            runState.wifiConnectState = WIFI_CONNECT_ERR;
            break;
        case 5://正在获取三元组
            runState.wifiConnectState = WIFI_CONNECT_OK;
            break;
        case 6://获取三元组成功
            runState.wifiConnectState = WIFI_CONNECT_OK;
            break;
        case 7://获取三元组失败
            runState.wifiConnectState = WIFI_CONNECT_OK;
            break;
        case 8://正在登录阿里
            runState.wifiConnectState = WIFI_CONNECT_OK;
            break;
        case 9://登录阿里成功
            runState.wifiConnectState = WIFI_CONNECT_OK;
            break;
        case 10://登录阿里失败
            runState.wifiConnectState = WIFI_CONNECT_OK;
            break;
        case 50://正在预注册
            runState.wifiConnectState = WIFI_CONNECT_OK;
            runState.wifiRegestState = WIFI_CONNECT_ING;
            break;
        case 51://预注册成功
            runState.wifiConnectState = WIFI_CONNECT_OK;
            runState.wifiRegestState = WIFI_CONNECT_OK;
            break;
        case 52://预注册失败
            runState.wifiConnectState = WIFI_CONNECT_OK;
            runState.wifiRegestState = WIFI_CONNECT_ERR;
            break;
        case 60://正在读取OTA文件
            runState.wifiConnectState = WIFI_CONNECT_OK;
            break;
        case 61://读取OTA成功
            runState.wifiConnectState = WIFI_CONNECT_OK;
            break;
        case 62://读取OTA失败
            runState.wifiConnectState = WIFI_CONNECT_OK;
            break;
        case 63://芯片ID错误
            runState.wifiConnectState = WIFI_CONNECT_OK;
            break;
        case 64://版本错误
            runState.wifiConnectState = WIFI_CONNECT_OK;
            break;
        case 65://正在下载
            runState.wifiOTAState = WIFI_CONNECT_ING;
            break;
        case 66://OTA成功
            runState.wifiOTAState = WIFI_CONNECT_OK;
            runState.wifiOTAState = WIFI_CONNECT_ING;
            break;
        case 67://OTA失败
            runState.wifiOTAState = WIFI_CONNECT_ERR;
            runState.wifiOTAState = WIFI_CONNECT_ING;
            break;
        }
        break;
    case WIFI_WORK_REG://注册
        switch(state)
        {
//        case 0://wifi正在连接
//            if(cmd2 == WIFI_CMD2_WIFI_CONTROL_CONNECT)  runState.wifiConnectState = WIFI_CONNECT_ING;
//            else                                        runState.wifiConnectState = WIFI_CONNECT_NULL;
//            break;
//        case 1://wifi初始化
//            break;
//        case 2://wifi正在连接
//            runState.wifiConnectState = WIFI_CONNECT_ING;
//            break;
//        case 3://wifi连接成功
//            runState.wifiConnectState = WIFI_CONNECT_OK;
//            break;
//        case 4://wifi连接失败
//            runState.wifiConnectState = WIFI_CONNECT_ERR;
//            break;
//        case 5://正在获取三元组
//            runState.wifiConnectState = WIFI_CONNECT_OK;
//            break;
//        case 6://获取三元组成功
//            runState.wifiConnectState = WIFI_CONNECT_OK;
//            break;
//        case 7://获取三元组失败
//            runState.wifiConnectState = WIFI_CONNECT_OK;
//            break;
//        case 8://正在登录阿里
//            runState.wifiConnectState = WIFI_CONNECT_OK;
//            break;
//        case 9://登录阿里成功
//            runState.wifiConnectState = WIFI_CONNECT_OK;
//            break;
//        case 10://登录阿里失败
//            runState.wifiConnectState = WIFI_CONNECT_OK;
//            break;
        case 50://正在预注册
            runState.wifiConnectState = WIFI_CONNECT_OK;
            runState.wifiRegestState = WIFI_CONNECT_ING;
            break;
        case 51://预注册成功
            runState.wifiConnectState = WIFI_CONNECT_OK;
            runState.wifiRegestState = WIFI_CONNECT_OK;
            break;
        case 52://预注册失败
            runState.wifiConnectState = WIFI_CONNECT_OK;
            runState.wifiRegestState = WIFI_CONNECT_ERR;
            break;
//        case 60://正在读取OTA文件
//            runState.wifiConnectState = WIFI_CONNECT_OK;
//            break;
//        case 61://读取OTA成功
//            runState.wifiConnectState = WIFI_CONNECT_OK;
//            break;
//        case 62://读取OTA失败
//            runState.wifiConnectState = WIFI_CONNECT_OK;
//            break;
//        case 63://芯片ID错误
//            runState.wifiConnectState = WIFI_CONNECT_OK;
//            break;
//        case 64://版本错误
//            runState.wifiConnectState = WIFI_CONNECT_OK;
//            break;
//        case 65://正在下载
//            runState.wifiOTAState = WIFI_CONNECT_ING;
//            break;
//        case 66://OTA成功
//            runState.wifiOTAState = WIFI_CONNECT_OK;
//            runState.wifiOTAState = WIFI_CONNECT_ING;
//            break;
//        case 67://OTA失败
//            runState.wifiOTAState = WIFI_CONNECT_ERR;
//            runState.wifiOTAState = WIFI_CONNECT_ING;
//            break;
        }
        break;
    case WIFI_WORK_OTA://OTA
        switch(state)
        {
//        case 0://wifi正在连接
//            if(cmd2 == WIFI_CMD2_WIFI_CONTROL_CONNECT)  runState.wifiConnectState = WIFI_CONNECT_ING;
//            else                                        runState.wifiConnectState = WIFI_CONNECT_NULL;
//            break;
//        case 1://wifi初始化
//            break;
//        case 2://wifi正在连接
//            runState.wifiConnectState = WIFI_CONNECT_ING;
//            break;
//        case 3://wifi连接成功
//            runState.wifiConnectState = WIFI_CONNECT_OK;
//            break;
//        case 4://wifi连接失败
//            runState.wifiConnectState = WIFI_CONNECT_ERR;
//            break;
//        case 5://正在获取三元组
//            runState.wifiConnectState = WIFI_CONNECT_OK;
//            break;
//        case 6://获取三元组成功
//            runState.wifiConnectState = WIFI_CONNECT_OK;
//            break;
//        case 7://获取三元组失败
//            runState.wifiConnectState = WIFI_CONNECT_OK;
//            break;
//        case 8://正在登录阿里
//            runState.wifiConnectState = WIFI_CONNECT_OK;
//            break;
//        case 9://登录阿里成功
//            runState.wifiConnectState = WIFI_CONNECT_OK;
//            break;
//        case 10://登录阿里失败
//            runState.wifiConnectState = WIFI_CONNECT_OK;
//            break;
//        case 50://正在预注册
//            runState.wifiConnectState = WIFI_CONNECT_OK;
//            runState.wifiRegestState = WIFI_CONNECT_ING;
//            break;
//        case 51://预注册成功
//            runState.wifiConnectState = WIFI_CONNECT_OK;
//            runState.wifiRegestState = WIFI_CONNECT_OK;
//            break;
//        case 52://预注册失败
//            runState.wifiConnectState = WIFI_CONNECT_OK;
//            runState.wifiRegestState = WIFI_CONNECT_ERR;
//            break;
        case 60://正在读取OTA文件
            runState.wifiConnectState = WIFI_CONNECT_OK;
            break;
        case 61://读取OTA成功
            runState.wifiConnectState = WIFI_CONNECT_OK;
            break;
        case 62://读取OTA失败
            runState.wifiConnectState = WIFI_CONNECT_OK;
            break;
        case 63://芯片ID错误
            runState.wifiConnectState = WIFI_CONNECT_OK;
            break;
        case 64://版本错误
            runState.wifiConnectState = WIFI_CONNECT_OK;
            break;
        case 65://正在下载
            runState.wifiOTAState = WIFI_CONNECT_ING;
            break;
        case 66://OTA成功
            runState.wifiOTAState = WIFI_CONNECT_OK;
            runState.wifiOTAState = WIFI_CONNECT_ING;
            break;
        case 67://OTA失败
            runState.wifiOTAState = WIFI_CONNECT_ERR;
            runState.wifiOTAState = WIFI_CONNECT_ING;
            break;
        }
        break;
    }
}

void UartData_Process(uint8_t da)
{
    uint8_t sum=0;
    uint16_t i=0;
    static uint16_t len=0;
    Wifi_Packet_t *wifi_pkt=NULL;
    TurnOn_Packet_t *turnon_pkt=NULL;
    WifiState_Packet_t *wifistate_pkt=NULL;
    WifiVer_Packet_t *wifiver=NULL;
    Sch_Task_t *sckt_pkt=NULL;
    Param_SetPacket_t *param_pkt=NULL;

    Rx3Buff[buf3_size++] = da;

    if (Rx3Buff[0] == 0xFE)
    {
        if (buf3_size >= 6)
        {
            if (Rx3Buff[0] == 0xFE && Rx3Buff[1] == 0xAA) // 接收到帧头
            {
                len = Rx3Buff[4]+Rx3Buff[5]*256;
                // 接收到帧尾
                if (buf3_size >= len)
                {
                    sum = 0;
                    for(i=0;i<len-1;i++)
                    {
                        sum = sum + Rx3Buff[i];
                    }
                    if(sum == Rx3Buff[len-1])
                    {
                        wifi_pkt = (Wifi_Packet_t*)Rx3Buff;
                        if(wifi_pkt->wifiProtocolVer==WIFI_PROTOCOL_VER)
                        {
                            switch(wifi_pkt->wifiCmd1)
                            {
                            case WIFI_CMD1_DEVICE_STATE://获取设备状态
                                runState.wifiSendDelayCnt = 0;
                                switch(wifi_pkt->wifiCmd2)
                                {
                                case WIFI_CMD2_DEVICE_STATE_NORMAL_PKT:
                                    runState.wifiSendPacketState = WIFI_SENDPKT_STATE_HEARTBEAT;
                                    break;
                                case WIFI_CMD2_DEVICE_STATE_DEBUG_PKT:
                                    runState.wifiSendPacketState = WIFI_SENDPKT_STATE_DIAGNOSIS;
                                    break;
                                case WIFI_CMD2_DEVICE_STATE_ERR_PKT:
                                    runState.wifiSendPacketState = WIFI_SENDPKT_STATE_DEVICE_ERR;
                                    break;
                                case WIFI_CMD2_DEVICE_STATE_MAC_PKT:
                                    runState.wifiSendPacketState = WIFI_SENDPKT_STATE_DEVICE_MAC_RESPONSE;
                                    break;
                                default:
                                    break;
                                }
                                break;
                            case WIFI_CMD1_DEVICE_CONTROL://设备控制 开关机
//                                if(baseParameters.wifiEn == 0)
//                                {
//
//                                }else
                                {
                                    runState.wifiSendDelayCnt = 0;
                                    switch(wifi_pkt->wifiCmd2)
                                    {
                                    case WIFI_CMD2_DEVICE_CONTROL_OFF://关机
                                        if(runState.disIndex!=DISINDEX_PWROFF_ANIMATION && runState.disIndex!=DISINDEX_PWROFFED)
                                        {
                                            if(runState.disIndex==DISINDEX_CO_OVERWARNING || runState.disIndex==DISINDEX_CO_OVER_DEVICE_OFFED)
                                            {
                                                clear_State();
                                            }else {
                                                TurnOff_Device();
                                            }
                                        }
                                        runState.wifiSendPacketState = WIFI_SENDPKT_STATE_TURN_OFF_RESPONSE;
                                        runState.bleSendPacketState = WIFI_SENDPKT_STATE_TURN_ONOFF_RESPONSE;
                                        break;
                                    case WIFI_CMD2_DEVICE_CONTROL_ON://开机
                                        runState.turnOnType = DEVICE_TURNON_WIFI;
                                        if(wifi_pkt->wifiPacketLen == 9)//无开机参数
                                        {
                                            if(runState.runModel != REMOTE_STATE_POWER_ON)  TurnOn_Device(runState.lastMode);  //遥控开机默认上次关机前状态
                                        }
                                        else if(wifi_pkt->wifiPacketLen == 13)//开机带参数
                                        {
        //                                            APP_DBG("Usart3 TurnOn Mode:%d Param:%d",*(&(wifi_pkt->wifiData)),*((&(wifi_pkt->wifiData))+1));
                                            turnon_pkt = (TurnOn_Packet_t*)(&(wifi_pkt->wifiData));
                                            if(turnon_pkt->turnOnMode == WIFI_CONTROL_MODE_LEVEL)//档位
                                            {
                                                if(turnon_pkt->turnOnParam>=POWER_LEVEL_NUM_MIN && turnon_pkt->turnOnParam<=POWER_LEVEL_NUM_MAX)
                                                {
                                                    runState.setLevel = turnon_pkt->turnOnParam;
                                                    if(runState.runModel != REMOTE_STATE_POWER_ON)
                                                        TurnOn_Device(CTRL_RUNMODE_LEVEL);
                                                    else
                                                        ControlModel_Change(CTRL_RUNMODE_LEVEL);
                                                }
                                            }else if(turnon_pkt->turnOnMode == WIFI_CONTROL_MODE_TEMP)//温控
                                            {
                                                if(GetDisSetTemperModeStandby() == 0)
                                                {
                                                    if(baseParameters.tempUnit == TEMPER_UINT_C)
                                                    {
                                                        if(turnon_pkt->turnOnParam>=CTRL_RUNMODE_TEMP_MIN && turnon_pkt->turnOnParam<=CTRL_RUNMODE_TEMP_MAX)
                                                        {
                                                            runState.setTemper = turnon_pkt->turnOnParam;
                                                            runState.setHTemper = (int16_t)(((float)runState.setTemper*1.8) + 32.0);
                                                            if(runState.runModel != REMOTE_STATE_POWER_ON)
                                                                TurnOn_Device(CTRL_RUNMODE_TEMP);
                                                            else
                                                                ControlModel_Change(CTRL_RUNMODE_TEMP);
                                                            CleardisSetTempBlinkCnt();
                                                            SetDisSetTemperBlink(1);
                                                            runState.bklight_time = GetConfigData_BledTime()*10;
                                                        }
                                                    }else if(baseParameters.tempUnit == TEMPER_UINT_F)
                                                    {
                                                        if(turnon_pkt->turnOnParam>=CTRL_RUNMODE_HTEMP_MIN && turnon_pkt->turnOnParam<=CTRL_RUNMODE_HTEMP_MAX)
                                                        {
                                                            runState.setHTemper = turnon_pkt->turnOnParam;
                                                            runState.setTemper = (int16_t)(((float)(runState.setHTemper-32))/1.8);
                                                            if(runState.runModel != REMOTE_STATE_POWER_ON)
                                                                TurnOn_Device(CTRL_RUNMODE_TEMP);
                                                            else
                                                                ControlModel_Change(CTRL_RUNMODE_TEMP);
                                                            CleardisSetTempBlinkCnt();
                                                            SetDisSetTemperBlink(1);
                                                            runState.bklight_time = GetConfigData_BledTime()*10;
                                                        }
                                                    }
                                                }
                                                else if(GetDisSetTemperModeStandby() == 1)
                                                {
                                                    if(baseParameters.tempUnit == TEMPER_UINT_C)
                                                    {
                                                        if(turnon_pkt->turnOnParam>=CTRL_RUNMODE_TEMP_MIN && turnon_pkt->turnOnParam<=CTRL_RUNMODE_TEMP_MAX)
                                                        {
                                                            baseParameters.temperModeStandby = 1;
                                                            runState.setTemper = turnon_pkt->turnOnParam;
                                                            runState.setHTemper = (int16_t)(((float)runState.setTemper*1.8) + 32.0);
                                                            if(runState.runModel != REMOTE_STATE_POWER_ON)
                                                                TurnOn_Device(CTRL_RUNMODE_AUTO);
                                                            else
                                                                ControlModel_Change(CTRL_RUNMODE_AUTO);
                                                            CleardisSetTempBlinkCnt();
                                                            SetDisSetTemperBlink(1);
                                                            runState.bklight_time = GetConfigData_BledTime()*10;
                                                        }
                                                    }else if(baseParameters.tempUnit == TEMPER_UINT_F)
                                                    {
                                                        if(turnon_pkt->turnOnParam>=CTRL_RUNMODE_HTEMP_MIN && turnon_pkt->turnOnParam<=CTRL_RUNMODE_HTEMP_MAX)
                                                        {
                                                            baseParameters.temperModeStandby = 1;
                                                            runState.setHTemper = turnon_pkt->turnOnParam;
                                                            runState.setTemper = (int16_t)(((float)(runState.setHTemper-32))/1.8);
                                                            if(runState.runModel != REMOTE_STATE_POWER_ON)
                                                                TurnOn_Device(CTRL_RUNMODE_AUTO);
                                                            else
                                                                ControlModel_Change(CTRL_RUNMODE_AUTO);
                                                            CleardisSetTempBlinkCnt();
                                                            SetDisSetTemperBlink(1);
                                                            runState.bklight_time = GetConfigData_BledTime()*10;
                                                        }
                                                    }
                                                }
                                            }else if(turnon_pkt->turnOnMode == WIFI_CONTROL_MODE_VENTILATE)//通风
                                            {
                                                if(turnon_pkt->turnOnParam>=POWER_LEVEL_NUM_MIN && turnon_pkt->turnOnParam<=POWER_LEVEL_NUM_MAX)
                                                {
                                                    runState.ventilateLevel = turnon_pkt->turnOnParam;
                                                    if(runState.runModel != REMOTE_STATE_POWER_ON)
                                                        TurnOn_Device(CTRL_RUNMODE_VENTILATE);
                                                    else
                                                        ControlModel_Change(CTRL_RUNMODE_VENTILATE);
                                                }
                                            }else if(turnon_pkt->turnOnMode == WIFI_CONTROL_MODE_FASTHEAT)//强热
                                            {
                                                if(runState.runModel != REMOTE_STATE_POWER_ON)
                                                    TurnOn_Device(CTRL_RUNMODE_FASTHEAT);
                                                else
                                                    ControlModel_Change(CTRL_RUNMODE_FASTHEAT);
                                            }
    //                                        else if(turnon_pkt->turnOnMode == WIFI_CONTROL_MODE_AUTO)//自启停
    //                                        {
    //                                            if(baseParameters.tempUnit == TEMPER_UINT_C)
    //                                            {
    //                                                if(turnon_pkt->turnOnParam>=CTRL_RUNMODE_TEMP_MIN && turnon_pkt->turnOnParam<=CTRL_RUNMODE_TEMP_MAX)
    //                                                {
    //                                                    baseParameters.temperModeStandby = 1;
    //                                                    runState.setTemper = turnon_pkt->turnOnParam;
    //                                                    runState.setHTemper = (int16_t)(((float)runState.setTemper*1.8) + 32.0);
    //                                                    if(runState.runModel != REMOTE_STATE_POWER_ON)
    //                                                        TurnOn_Device(CTRL_RUNMODE_AUTO);
    //                                                    else
    //                                                        ControlModel_Change(CTRL_RUNMODE_AUTO);
    //                                                    CleardisSetTempBlinkCnt();
    //                                                    SetDisSetTemperBlink(1);
    //                                                    runState.bklight_time = GetConfigData_BledTime()*10;
    //                                                }
    //                                            }else if(baseParameters.tempUnit == TEMPER_UINT_F)
    //                                            {
    //                                                if(turnon_pkt->turnOnParam>=CTRL_RUNMODE_HTEMP_MIN && turnon_pkt->turnOnParam<=CTRL_RUNMODE_HTEMP_MAX)
    //                                                {
    //                                                    baseParameters.temperModeStandby = 1;
    //                                                    runState.setHTemper = turnon_pkt->turnOnParam;
    //                                                    runState.setTemper = (int16_t)(((float)(runState.setHTemper-32))/1.8);
    //                                                    if(runState.runModel != REMOTE_STATE_POWER_ON)
    //                                                        TurnOn_Device(CTRL_RUNMODE_AUTO);
    //                                                    else
    //                                                        ControlModel_Change(CTRL_RUNMODE_AUTO);
    //                                                    CleardisSetTempBlinkCnt();
    //                                                    SetDisSetTemperBlink(1);
    //                                                    runState.bklight_time = GetConfigData_BledTime()*10;
    //                                                }
    //                                            }
    //                                        }
        //                                            APP_DBG("Usart3 TurnOn Mode:%d Param:%d Time:0x%x",turnon_pkt->turnOnMode,turnon_pkt->turnOnParam,turnon_pkt->turnOnMinute);
                                        }
                                        runState.wifiSendPacketState = WIFI_SENDPKT_STATE_TURN_ON_RESPONSE;
                                        runState.bleSendPacketState = WIFI_SENDPKT_STATE_TURN_ONOFF_RESPONSE;
                                        break;
                                    default:
                                        break;
                                    }
                                }
                                break;
                            case WIFI_CMD1_DEVICE_SCH_TASK://定时任务
//                                if(baseParameters.wifiEn == 0)
//                                {
//
//                                }else
                                {
                                    runState.wifiSendDelayCnt = 0;
                                    switch(wifi_pkt->wifiCmd2)
                                    {
                                    case WIFI_CMD2_DEVICE_SCH_TASK_QUERY://查询
                                        runState.wifiSendDelayCnt = 0;
                                        runState.wifiSendPacketState = WIFI_SENDPKT_STATE_SCHTASK_QUERY_RESPONSE;
                                        break;
                                    case WIFI_CMD2_DEVICE_SCH_TASK_ADD://新增
        //                                        runState.wifiSendDelayCnt = 0;
        //                                        runState.wifiSendPacketState = WIFI_SENDPKT_STATE_SCHTASK_ADD_RESPONSE;
                                        break;
                                    case WIFI_CMD2_DEVICE_SCH_TASK_EDIT://修改
                                        if(wifi_pkt->wifiData == 1)//只接收定时数为1的数据
                                        {
                                            sckt_pkt = (Sch_Task_t*)(((&(wifi_pkt->wifiData))+1));

                                            if(sckt_pkt->schTaskEn>=0 && sckt_pkt->schTaskEn<=1)
                                                baseParameters.timerClockEn = sckt_pkt->schTaskEn;
                                            if(sckt_pkt->schTaskStartHour>=0 && sckt_pkt->schTaskStartHour<=23)
                                                baseParameters.timerClockHour = sckt_pkt->schTaskStartHour;
                                            if(sckt_pkt->schTaskStartMinute>=0 && sckt_pkt->schTaskStartMinute<=59)
                                                baseParameters.timerClockMinute = sckt_pkt->schTaskStartMinute;
                                            if(sckt_pkt->schTaskRunMinute>=0 && sckt_pkt->schTaskRunMinute<=600)
                                                baseParameters.totalHeatTime = sckt_pkt->schTaskRunMinute;
                                            runState.wifiSendDelayCnt = 0;
                                            runState.wifiSendPacketState = WIFI_SENDPKT_STATE_SCHTASK_EDIT_RESPONSE;
                                            runState.bleSendPacketState = WIFI_SENDPKT_STATE_SCHTASK_CHANGE_RESPONSE;

                                            vPARA_SAVE(0);
                                        }
                                        break;
                                    case WIFI_CMD2_DEVICE_SCH_TASK_DEL://删除
        //                                        runState.wifiSendDelayCnt = 0;
        //                                        runState.wifiSendPacketState = WIFI_SENDPKT_STATE_SCHTASK_DEL_RESPONSE;
                                        break;
                                    default:
                                        break;
                                    }
                                }
                                break;
                            case WIFI_CMD1_DEVICE_PARAM://属性设置
//                                if(baseParameters.wifiEn == 0)
//                                {
//                                    switch(wifi_pkt->wifiCmd2)
//                                    {
//                                    case WIFI_CMD2_DEVICE_PARAM_SET://设置
//                                        //修改属性值
//                                        param_pkt = (Param_SetPacket_t*)(&(wifi_pkt->wifiData));
//                                        if(param_pkt->paramDeviceTime&0x8000)
//                                        {
//                                        }else
//                                        {
//                                            //wifi使能
//                                            if(param_pkt->paramWifiEn<= 1)
//                                                baseParameters.wifiEn = param_pkt->paramWifiEn;
//                                        }
//                                    }
//                                    vPARA_SAVE(0);
//                                }else
                                {
                                    runState.wifiSendDelayCnt = 0;
                                    switch(wifi_pkt->wifiCmd2)
                                    {
                                    case WIFI_CMD2_DEVICE_PARAM_QUERY://查询
                                        runState.wifiSendDelayCnt = 0;
                                        runState.wifiSendPacketState = WIFI_SENDPKT_STATE_ATTRIBUTE_GET_RESPONSE;
                                        break;
                                    case WIFI_CMD2_DEVICE_PARAM_SET://设置
                                        //修改属性值
                                        param_pkt = (Param_SetPacket_t*)(&(wifi_pkt->wifiData));
                                        if(param_pkt->paramDeviceTime&0x8000)
                                        {
                                            if((param_pkt->paramDeviceTime&0x0FFF)<=1440)
                                            {
                                                SetDisSetTime((param_pkt->paramDeviceTime&0x0FFF)/60,(param_pkt->paramDeviceTime&0x0FFF)%60);
                                            }
                                        }else
                                        {
                                            //海拔单位
                                            if(param_pkt->paramAltitudeUnit<ALTITUDE_UINT_MAX && baseParameters.altitudeUnit<ALTITUDE_UINT_MAX)
                                                baseParameters.altitudeUnit = param_pkt->paramAltitudeUnit;
                                            //温度单位
                                            if(param_pkt->paramTTempUnit<TEMPER_UINT_MAX && baseParameters.tempUnit<TEMPER_UINT_MAX)
                                                baseParameters.tempUnit = param_pkt->paramTTempUnit;
                                            //当前时间
    //                                        if((param_pkt->paramDeviceTime&0x0FFF)<=1440)
    //                                            SetDisSetTime((param_pkt->paramDeviceTime&0x0FFF)/60,(param_pkt->paramDeviceTime&0x0FFF)%60);

                                            //温度偏差
                                            if(baseParameters.tempUnit == TEMPER_UINT_C)
                                            {
                                                if(param_pkt->paramTempOffset>=TEMPC_OFFSET_MIN && param_pkt->paramTempOffset<=TEMPC_OFFSET_MAX)
                                                {
                                                    baseParameters.temperSensorOffset = param_pkt->paramTempOffset;
                                                    baseParameters.temperMidOffset = TempMul18(baseParameters.temperSensorOffset);//(int8_t)(temp*1.8);
                                                }
                                            }else if(baseParameters.tempUnit == TEMPER_UINT_F)
                                            {
                                                if(param_pkt->paramTempOffset>=TEMPF_OFFSET_MIN && param_pkt->paramTempOffset<=TEMPF_OFFSET_MAX)
                                                {
                                                    baseParameters.temperMidOffset = param_pkt->paramTempOffset;
                                                    baseParameters.temperSensorOffset = TempDiv18(baseParameters.temperMidOffset);//(int8_t)(temp/1.8);
                                                }
                                            }
                                            //语音
                                            if(param_pkt->paramLanguageType<LANGUAGE_TYPE_MAX)
                                            {
#if XINMAIZHONG==1
                                                if(param_pkt->paramLanguageType==4) baseParameters.languageType = 2;
                                                else                                baseParameters.languageType = param_pkt->paramLanguageType;
#else
                                                baseParameters.languageType = param_pkt->paramLanguageType;
#endif
                                            }
                                            //背光亮度
                                            if(param_pkt->paramBledLevel <= BLED_LEVEL_MAX)
                                                baseParameters.bledDuty = param_pkt->paramBledLevel;
                                            //开、关机温度差
                                            if(baseParameters.tempUnit == TEMPER_UINT_C)
                                            {
                                                APP_DBG("OnC:%d OffC:%d",param_pkt->paramTempOnDvi,param_pkt->paramTempOffDvi);
                                                if(param_pkt->paramTempOnDvi>=TEMPC_ONOFF_MIN && param_pkt->paramTempOnDvi<=TEMPC_ONOFF_MAX)
                                                {
                                                    baseParameters.tempOnDvi = param_pkt->paramTempOnDvi;
                                                    baseParameters.tempOnHDvi = TempMul18(baseParameters.tempOnDvi);
                                                }
                                                if(param_pkt->paramTempOffDvi>=TEMPC_ONOFF_MIN && param_pkt->paramTempOffDvi<=TEMPC_ONOFF_MAX)
                                                {
                                                    baseParameters.tempOffDvi = param_pkt->paramTempOffDvi;
                                                    baseParameters.tempOffHDvi = TempMul18(baseParameters.tempOffDvi);
                                                }
                                            }else
                                            {
                                                APP_DBG("OnH:%d OffH:%d",param_pkt->paramTempOnDvi,param_pkt->paramTempOffDvi);
                                                if(param_pkt->paramTempOnDvi>=TEMPH_ONOFF_MIN && param_pkt->paramTempOnDvi<=TEMPH_ONOFF_MAX)
                                                {
                                                    baseParameters.tempOnHDvi = param_pkt->paramTempOnDvi;
                                                    baseParameters.tempOnDvi = TempDiv18(baseParameters.tempOnHDvi);
                                                }
                                                if(param_pkt->paramTempOffDvi>=TEMPH_ONOFF_MIN && param_pkt->paramTempOffDvi<=TEMPH_ONOFF_MAX)
                                                {
                                                    baseParameters.tempOffHDvi = param_pkt->paramTempOffDvi;
                                                    baseParameters.tempOffDvi = TempDiv18(baseParameters.tempOffHDvi);
                                                }
                                            }

                                            //wifi使能
                                            if(param_pkt->paramWifiEn<= 1)
                                                baseParameters.wifiEn = param_pkt->paramWifiEn;

                                            if(param_pkt->autoOnOff <= 1)
                                            {
                                                baseParameters.temperModeStandby = param_pkt->autoOnOff;
                                                if(baseParameters.runMode == CTRL_RUNMODE_TEMP)
                                                {
                                                    if(baseParameters.temperModeStandby)    ControlModel_Change(CTRL_RUNMODE_AUTO);
                                                }else if(baseParameters.runMode == CTRL_RUNMODE_AUTO)
                                                {
                                                    if(baseParameters.temperModeStandby == 0)    ControlModel_Change(CTRL_RUNMODE_TEMP);
                                                }
                                            }
                                            vPARA_SAVE(0);
                                        }


                                        runState.wifiSendDelayCnt = 0;
                                        runState.wifiSendPacketState = WIFI_SENDPKT_STATE_ATTRIBUTE_SET_RESPONSE;
                                        runState.bleSendPacketState = WIFI_SENDPKT_STATE_ATTRIBUTE_CHANGE_RESPONSE;
                                        break;
                                    case WIFI_CMD2_DEVICE_PARAM_REST://恢复出厂
                                        //所有参数恢复默认
                                        baseParameters.init = 0xFF;
                                        vPARA_SAVE(1);
                                        SYS_ResetExecute();
                                        runState.wifiSendDelayCnt = 0;
                                        runState.wifiSendPacketState = WIFI_SENDPKT_STATE_ATTRIBUTE_RESET_RESPONSE;
                                        runState.bleSendPacketState = WIFI_SENDPKT_STATE_ATTRIBUTE_CHANGE_RESPONSE;
                                        break;
                                    default:
                                        break;
                                    }
                                }

                                break;
                            case WIFI_CMD1_WIFI_CONTROL://wifi操作
                                switch(wifi_pkt->wifiCmd2)
                                {
                                case WIFI_CMD2_WIFI_CONTROL_QUERY://查询状态返回
                                    wifistate_pkt = (WifiState_Packet_t*)(&(wifi_pkt->wifiData));
                                    runState.wifiWorkStep = wifistate_pkt->wifiConnectState;
                                    runState.wifiWorkState = wifistate_pkt->wifiWorkState;
                                    runState.wifiSaveState = wifistate_pkt->wifiSaveState;
//                                    runState.wifiRegestState = wifistate_pkt->wifiRegState;
                                    runState.wifiClearBindState = wifistate_pkt->wifiClearState;
                                    runState.wifiOTAProgress = wifistate_pkt->wifiOTAProgress;
                                    if(runState.wifiWorkState == WIFI_WORK_CONN)
                                        wifiState_Get(wifistate_pkt->wifiWorkState,wifi_pkt->wifiCmd2,wifistate_pkt->wifiApState);
                                    else if(runState.wifiWorkState == WIFI_WORK_THING)
                                    {
                                        runState.wifiRegestState = wifistate_pkt->wifiRegState;
                                        wifiState_Get(wifistate_pkt->wifiWorkState,wifi_pkt->wifiCmd2,wifistate_pkt->wifiConnectState);
                                    }
                                    else
                                    {
                                        wifiState_Get(wifistate_pkt->wifiWorkState,wifi_pkt->wifiCmd2,wifistate_pkt->wifiConnectState);
                                    }
                                    if(runState.wifiOTAProgress==100)
                                    {
                                        runState.disSetTimeState = DISSETTIME_STATE_NULL;
                                        runState.wifiOTAState = WIFI_CONNECT_OK;
                                    }
                                    if(runState.wifiRegestState!=WIFI_CONNECT_OK && runState.wifiRegStartCnt!=0)
                                    {
                                        runState.wifiRegStartCnt ++;
                                        if(runState.wifiRegStartCnt>=3)
                                        {
                                            //强制跳转注册页面，并开始预注册
                                            runState.wifiRegStartCnt = 0;
                                            runState.disIndex = DISINDEX_FAST_REG_WIFI;//快速wifi预注册
                                            runState.wifiConnectInitMode = 0;
                                            runState.wifiRegestState = WIFI_CONNECT_NULL;
                                            wifiOnoff(1);
                                            //打开背光
                                            Lcd_Bk_On();
                                        }
                                    }
                                    if(runState.wifiRegestState == WIFI_CONNECT_OK)
                                    {
                                        runState.wifiRegStartCnt = 0;
                                    }
                                    break;
                                case WIFI_CMD2_WIFI_CONTROL_CONNECT://配网返回
                                    wifistate_pkt = (WifiState_Packet_t*)(&(wifi_pkt->wifiData));
                                    if(wifistate_pkt->wifiApState == 8)       runState.wifiConnectState = WIFI_CONNECT_OK;
                                    else if(wifistate_pkt->wifiApState == 7)  runState.wifiConnectState = WIFI_CONNECT_ERR;
                                    else if(wifistate_pkt->wifiApState == 6)  runState.wifiConnectState = WIFI_CONNECT_ING;
                                    else if(wifistate_pkt->wifiApState == 5)  runState.wifiConnectState = WIFI_CONNECT_ERR;
                                    runState.wifiWorkState = wifistate_pkt->wifiWorkState;
                                    runState.wifiSaveState = wifistate_pkt->wifiSaveState;
                                    runState.wifiRegestState = wifistate_pkt->wifiRegState;
                                    runState.wifiClearBindState = wifistate_pkt->wifiClearState;
                                    break;
                                case WIFI_CMD2_WIFI_CONTROL_CLEAR://清除绑定
                                    wifistate_pkt = (WifiState_Packet_t*)(&(wifi_pkt->wifiData));
                                    wifiState_Get(wifistate_pkt->wifiWorkState,wifi_pkt->wifiCmd2,wifistate_pkt->wifiConnectState);
                                    break;
                                case WIFI_CMD2_WIFI_CONTROL_REGISTER://预注册
                                    wifistate_pkt = (WifiState_Packet_t*)(&(wifi_pkt->wifiData));
    //                                        runState.wifiConnectState = wifistate_pkt->wifiConnectState;

                                    runState.wifiWorkState = wifistate_pkt->wifiWorkState;
                                    runState.wifiSaveState = wifistate_pkt->wifiSaveState;
//                                    runState.wifiRegestState = wifistate_pkt->wifiRegState;
                                    runState.wifiClearBindState = wifistate_pkt->wifiClearState;

                                    wifiState_Get(wifistate_pkt->wifiWorkState,wifi_pkt->wifiCmd2,wifistate_pkt->wifiConnectState);
                                    break;
                                case WIFI_CMD2_WIFI_CONTROL_OTA://OTA
                                    //未处于OTA状态显示直接跳转，并实时查询进度
                                    if(runState.disIndex==DISINDEX_CFGCTRLPARAM_WIFI && runState.disSetTimeState==WIFI_SET_DISINDEX_OTA)
                                    {
                                        wifistate_pkt = (WifiState_Packet_t*)(&(wifi_pkt->wifiData));
                                        runState.wifiWorkState = wifistate_pkt->wifiWorkState;
                                        runState.wifiSaveState = wifistate_pkt->wifiSaveState;
                                        runState.wifiRegestState = wifistate_pkt->wifiRegState;
                                        runState.wifiClearBindState = wifistate_pkt->wifiClearState;
                                        runState.wifiOTAProgress = wifistate_pkt->wifiOTAProgress;//OTA进度
                                        if(runState.wifiOTAProgress==100)
                                        {
                                            runState.disSetTimeState = DISSETTIME_STATE_NULL;
                                            runState.wifiOTAState = WIFI_CONNECT_OK;
                                        }
                                    }else {
                                        Lcd_Bk_On();
                                        runState.disIndex = DISINDEX_CFGCTRLPARAM_WIFI;
                                        runState.disSetTimeState = WIFI_SET_DISINDEX_OTA;
                                        runState.wifiSendDelayCnt = 10;
                                        runState.wifiSendPacketState = WIFI_SENDPKT_WIFI_CONTROL_OTA;
                                        runState.wifiOTAState = WIFI_CONNECT_ING;
                                        runState.wifiDisIndex = WIFI_SET_DISINDEX_OTA;
                                    }
                                    break;
                                case WIFI_CMD2_WIFI_CONTROL_GETVER:
                                    wifiver = (WifiVer_Packet_t*)(&(wifi_pkt->wifiData));
                                    runState.wifiVer = wifiver->wifiV;
//                                    runState.wifiVer = wifi_pkt->wifiData;
//                                    APP_DBG("wVer:%d",wifi_pkt->wifiData);
                                    break;
                                default:
                                    break;
                                }
                                break;
                            default:
                                break;
                            }
                        }
                    }
                    buf3_size = 0;
                    len = 0;
                }
            }
            else
            {
                buf3_size = 0;
            }
        }
    }
    else
    {
        buf3_size = 0;
    }
}
/*********************************************************************
 * @fn      UART3_IRQHandler
 *
 * @brief   UART3中断函数
 *
 * @return  none
 */
__INTERRUPT
__HIGH_CODE
void UART3_IRQHandler(void)
{
    uint8_t dat;
//    uint8_t r[10];

    switch (UART3_GetITFlag())
    {
    case UART_II_LINE_STAT: // 线路状态错误
        dat = UART3_GetLinSTA();
        break;
    case UART_II_RECV_RDY: // 数据达到设置触发点
        while(R8_UART3_RFC>1)
        {
            dat = R8_UART3_RBR;
            UartData_Process(dat);
        }
        break;

//        i = UART3_RecvString(r);
//
//        Rx3Buff[buf3_size] = r[0];
//        buf3_size++;
//        if (Rx3Buff[0] == 0xFE)
//        {
//            if (buf3_size >= 6)
//            {
//
//            }
//        }
//        else
//        {
//            buf3_size = 0;
////            memset(Rx3Buff, 0, sizeof(Rx3Buff));
//        }
//
//        if (buf3_size >= sizeof(Rx3Buff))
//        {
//            buf3_size = 0;
////            memset(Rx3Buff, 0, sizeof(Rx3Buff));
//        }
        break;
    case UART_II_RECV_TOUT: // 接收超时，暂时一帧数据接收完成
        while(R8_UART3_RFC)
        {
            dat = R8_UART3_RBR;
            UartData_Process(dat);
        }

        break;
    case UART_II_THR_EMPTY: // 发送缓存区空，可继续发送
        break;
    case UART_II_MODEM_CHG: // 只支持串口0
        break;
    default:
        break;
    }
}

uint8_t Make_GetMACPacket(uint8_t* buf)
{
    uint8_t i=0;
    uint8_t mac[6];

    GetMACAddress(mac);

    GetMACPacket_t *mac_pkt = (GetMACPacket_t*)buf;
    mac_pkt->deviceID = DEVICE_TYPE;
    mac_pkt->deviceHW = 0;
    mac_pkt->deviceSW = VERSION;
    mac_pkt->languageEn = baseParameters.languageEn;//语言包含标志
    memcpy(mac_pkt->bleMac,mac,6);

    return sizeof(GetMACPacket_t);
}

uint16_t Make_NormalPacket(uint8_t* buf)
{
    static uint16_t lesstime = 0xFFFF;
    BaseUartPacket_t *bs_pkt = (BaseUartPacket_t*)buf;

    if(runState.mainBoardType == MAIN_BOARD_TYPE_VENTILATE_FASTHART)
        bs_pkt->mainBoardType = 1;
    else if(runState.mainBoardType == MAIN_BOARD_TYPE_NONE_VENTILATE)
        bs_pkt->mainBoardType = 0;
    if(runState.meshDeviceNum != 0)
        bs_pkt->meshDevNum = runState.meshDeviceNum-1;
    else
        bs_pkt->meshDevNum = 0;

    if(runState.disIndex==DISINDEX_PWROFF_ANIMATION || runState.disIndex==DISINDEX_OFFING)
    {
        bs_pkt->deviceState = 3;//散热
    }else
    {
        if(heaterDatas.faultType==0)
        {
            if(runState.disIndex==DISINDEX_CO_OVERWARNING || runState.disIndex==DISINDEX_CO_OVER_DEVICE_OFFED)
            {
                bs_pkt->deviceState = 6;//故障
            }else
            {
                if(runState.runModel == REMOTE_STATE_POWER_OFF)
                    bs_pkt->deviceState = 1;//关
                else if(runState.runModel == REMOTE_STATE_POWER_ON)
                {
                    if(runState.standbyOnOffDisState == TEMPER_STADNBY_STATE_OFF)
                        bs_pkt->deviceState = 5;//待机
                    else if(baseParameters.runMode == CTRL_RUNMODE_VENTILATE)
                        bs_pkt->deviceState = 6;//通风
                    else
                        bs_pkt->deviceState = 2;//开
                }
                else if(runState.runModel == REMOTE_STATE_PUMPOIL)
                    bs_pkt->deviceState = 4;//泵油
            }
        }else
        {
            bs_pkt->deviceState = 6;//故障
        }
    }

    switch(baseParameters.runMode)
    {
    case CTRL_RUNMODE_LEVEL:
        bs_pkt->deviceMode = 1;
        bs_pkt->deviceParam = runState.setLevel;
        bs_pkt->deviceLevel = runState.setLevel;
        break;
    case CTRL_RUNMODE_TEMP:
        bs_pkt->deviceMode = 2;
        if(baseParameters.tempUnit == TEMPER_UINT_C)        bs_pkt->deviceParam = (uint8_t)runState.setTemper;
        else if(baseParameters.tempUnit == TEMPER_UINT_F)   bs_pkt->deviceParam = (uint8_t)runState.setHTemper;
        bs_pkt->deviceLevel = GetRunningAutoSetLevel();
        break;
    case CTRL_RUNMODE_AUTO:
        bs_pkt->deviceMode = 2;
        if(baseParameters.tempUnit == TEMPER_UINT_C)        bs_pkt->deviceParam = (uint8_t)runState.setTemper;
        else if(baseParameters.tempUnit == TEMPER_UINT_F)   bs_pkt->deviceParam = (uint8_t)runState.setHTemper;
        bs_pkt->deviceLevel = GetRunningAutoSetLevel();
        break;
    case CTRL_RUNMODE_VENTILATE:
        bs_pkt->deviceMode = 3;
        bs_pkt->deviceParam = runState.ventilateLevel;
        bs_pkt->deviceLevel = runState.ventilateLevel;
        break;
    case CTRL_RUNMODE_FASTHEAT:
        bs_pkt->deviceMode = 4;
        bs_pkt->deviceParam = POWER_LEVEL_NUM_MAX+1;
        bs_pkt->deviceLevel = POWER_LEVEL_NUM_MAX+1;
        break;
    default:
        bs_pkt->deviceMode = 0;
        bs_pkt->deviceParam = 0;
        bs_pkt->deviceLevel = 0;
        break;
    }

    bs_pkt->mainBoardStep = heaterDatas.runningStep;
#if XINMAIZHONG==1
    switch(heaterDatas.faultType)
    {
    case Fault_Power:
        bs_pkt->dispErrcode = 1;
        break;
    case Fault_Ignition:
        bs_pkt->dispErrcode = 3;
        break;
    case Fault_OilPump:
        bs_pkt->dispErrcode = 4;
        break;
    case Fault_ShellTemper_Protect:
        bs_pkt->dispErrcode = 5;
        break;
    case Fault_WindMotor:
        bs_pkt->dispErrcode = 6;
        break;
    case Fault_IntAirTemper_Senser:
        bs_pkt->dispErrcode = 7;
        break;
    case Fault_Flameout:
        bs_pkt->dispErrcode = 8;
        break;
    case Fault_ShellTemper_Protect_Senser:
        bs_pkt->dispErrcode = 9;
        break;
    case Fault_Start:
        bs_pkt->dispErrcode = 10;
        break;
    default:
        if(runState.disIndex==DISINDEX_CO_OVERWARNING || runState.disIndex==DISINDEX_CO_OVER_DEVICE_OFFED)
        {
            bs_pkt->dispErrcode = 11;
        }else
        {
            bs_pkt->dispErrcode = heaterDatas.faultType;
        }
        break;
    }
#else
    if(runState.disIndex==DISINDEX_CO_OVERWARNING || runState.disIndex==DISINDEX_CO_OVER_DEVICE_OFFED)
    {
        bs_pkt->dispErrcode = 11;
    }else
    {
        bs_pkt->dispErrcode = heaterDatas.faultType;
    }
#endif
    bs_pkt->errCodeType = ERROR_CODE_TYPE;  //0x81通用故障码  0x80鑫迈众故障码
    bs_pkt->tempUnit = baseParameters.tempUnit;
    if(baseParameters.tempUnit == TEMPER_UINT_C)
    {
        bs_pkt->deviceTemp = GetTemperatureValue()/10;
        bs_pkt->tempOffSet = baseParameters.temperSensorOffset;//温度补偿
    }
    else if(baseParameters.tempUnit == TEMPER_UINT_F)
    {
        bs_pkt->deviceTemp = TempCtoF(GetTemperatureValue()/10);
        bs_pkt->tempOffSet = baseParameters.temperMidOffset;//温度补偿
    }
    bs_pkt->altitudeUnit = baseParameters.altitudeUnit;

    bs_pkt->altitude = (heaterDatas.altitude==0) ?  AltitudeChange(heaterDatas.cabAltitude,baseParameters.altitudeUnit) : AltitudeChange(heaterDatas.altitude/10,baseParameters.altitudeUnit);
    bs_pkt->supVoltage = heaterDatas.supplyVoltage;
    bs_pkt->shellTemp = TempChange(heaterDatas.shellTemp/10,baseParameters.tempUnit)*10;
    if(runState.meshDeviceState == REMOTE_STATE_POWER_ON)
    {
        bs_pkt->coNum = runState.meshDeviceCONum*10;//runState.coNum;
    }else
    {
        bs_pkt->coNum = 0xFFFF;
    }
    bs_pkt->turnOnType = runState.turnOnType;//开机触发方式
    bs_pkt->mainBoardHW = 1;//主板硬件版本
    bs_pkt->mainBoardSW = heaterDatas.version;//主板软件版本

#if XINMAIZHONG==1
    if(baseParameters.languageType==2)  bs_pkt->languageType = 4;
    else                                bs_pkt->languageType = baseParameters.languageType;//语言类型
#else
    bs_pkt->languageType = baseParameters.languageType;//语言类型
#endif
    bs_pkt->oilCaseVolume = 0xFF;//baseParameters.oilCaseVolume;//油箱大小
    bs_pkt->oilPumpType = 0xFF;//baseParameters.oilPumpType;//油泵类型
    bs_pkt->bkNum = baseParameters.bledDuty;//占空比需做下转换
    if(baseParameters.tempUnit == TEMPER_UINT_C)
    {
        bs_pkt->tempOnDvi = baseParameters.tempOnDvi;//开机温度差
        bs_pkt->tempOffDvi = baseParameters.tempOffDvi;//关机温度差
    }else if(baseParameters.tempUnit == TEMPER_UINT_F)
    {
        bs_pkt->tempOnDvi = baseParameters.tempOnHDvi;//开机温度差
        bs_pkt->tempOffDvi = baseParameters.tempOffHDvi;//关机温度差
    }

    bs_pkt->wifiEn = baseParameters.wifiEn;//wifi使能标志
    bs_pkt->autoOnOff = baseParameters.temperModeStandby;//自动启停标志
    bs_pkt->mainBoardRunMode = heaterDatas.runningMode;
    bs_pkt->runLessTime = lesstime;
    lesstime --;
    if(heaterDatas.runningLevel == 11)  bs_pkt->mainBoardRunMode = 21; //强热模式

    return sizeof(BaseUartPacket_t);
//    UART3_SendPacket(sizeof(BaseUartPacket_t),buf);
}

//诊断数据包
uint16_t Make_DebugPacket(uint8_t* buf)
{
    MainBoard_DebugPacket_t *dbg_pkt = (MainBoard_DebugPacket_t*)buf;

//    dbg_pkt->mainBoardRunMode = heaterDatas.runningMode;
//    if(heaterDatas.runningLevel == 11)  dbg_pkt->mainBoardRunMode = 21; //强热模式
    dbg_pkt->mainBoardRunLevel = heaterDatas.runningLevel;//主机运行档位
    dbg_pkt->mainBoardRunTime = heaterDatas.runTime;//主机运行时间
    dbg_pkt->ignitionVoltage = heaterDatas.ignitionVoltage;//点火塞电压
    dbg_pkt->ignitionPower = heaterDatas.ignitionPower;//点火塞功率
    dbg_pkt->ignitionCurrent = heaterDatas.ignitionCurrent;//点火塞电流
    dbg_pkt->ignitionRes = heaterDatas.ignitionRes;//点火塞内阻
    dbg_pkt->windCurrent = 0;//风机电流
    dbg_pkt->windSpeed = heaterDatas.windSpeed;//风机转速

    return sizeof(MainBoard_DebugPacket_t);
}

//定时任务数据包
uint16_t Make_SchTaskPacket(uint8_t* buf)
{
    Sch_TaskPacket_t *sch_pkt = (Sch_TaskPacket_t*)buf;

    sch_pkt->schTaskNum = SCH_TASK_NUM;
    sch_pkt->schTaskBuf[0].schTaskWeek = 0xFF;
    sch_pkt->schTaskBuf[0].schTaskEn = baseParameters.timerClockEn;
    sch_pkt->schTaskBuf[0].schTaskStartHour = baseParameters.timerClockHour;
    sch_pkt->schTaskBuf[0].schTaskStartMinute = baseParameters.timerClockMinute;
    sch_pkt->schTaskBuf[0].schTaskRunMinute = baseParameters.totalHeatTime;
    sch_pkt->schTaskBuf[0].schTaskRunMode = 0xFF;
    sch_pkt->schTaskBuf[0].schTaskRunParam = 0xFF;

    return sizeof(Sch_TaskPacket_t);
}

//wifi控制数据包
uint16_t Make_WifiControlPacket(uint8_t* buf)
{
    MainBoard_DebugPacket_t *dbg_pkt = (MainBoard_DebugPacket_t*)buf;

    dbg_pkt->mainBoardRunLevel = heaterDatas.runningLevel;//主机运行档位
    dbg_pkt->mainBoardRunTime = heaterDatas.runTime;//主机运行时间
    dbg_pkt->ignitionVoltage = heaterDatas.ignitionVoltage;//点火塞电压
    dbg_pkt->ignitionPower = heaterDatas.ignitionPower;//点火塞功率
    dbg_pkt->ignitionCurrent = heaterDatas.ignitionCurrent;//点火塞电流
    dbg_pkt->ignitionRes = heaterDatas.ignitionRes;//点火塞内阻
    dbg_pkt->windCurrent = 0;//风机电流
    dbg_pkt->windSpeed = heaterDatas.windSpeed;//风机转速

    return sizeof(MainBoard_DebugPacket_t);
}

void Uart3_SendPacket(uint8_t pkt_type)
{
    uint8_t buff[512] = { 0 };
    uint16_t slen=0,i=0;
    uint8_t sum=0;

    buff[0] = 0xFE;//Head1
    buff[1] = 0xAA;//Head2
    buff[2] = WIFI_PROTOCOL_VER1;//协议版本号
    buff[3] = 0x00;//包序号
    slen = 0;
    switch(pkt_type)
    {
    case WIFI_SENDPKT_STATE_HEARTBEAT://常用数据
        if(heaterDatas.faultType == 0)
        {
            buff[6] = 0x00+0x80;
            buff[7] = 0x00;
            slen = Make_NormalPacket(&buff[8]);
        }else
        {
            buff[6] = 0x00+0x80;
            buff[7] = 0x02;
            slen = Make_NormalPacket(&buff[8]);
            slen += Make_DebugPacket(&buff[8+slen]);
        }
        break;
    case WIFI_SENDPKT_STATE_DIAGNOSIS://诊断数据
        buff[6] = 0x00+0x80;
        buff[7] = 0x01;
        slen = Make_NormalPacket(&buff[8]);
        slen += Make_DebugPacket(&buff[8+slen]);
        break;
    case WIFI_SENDPKT_STATE_DEVICE_ERR://故障时上传数据
        buff[6] = 0x00+0x80;
        buff[7] = 0x02;
        slen = Make_NormalPacket(&buff[8]);
        slen += Make_DebugPacket(&buff[8+slen]);
        break;
    case WIFI_SENDPKT_STATE_DEVICE_MAC_RESPONSE://获取设备MAC应答
        buff[6] = 0x00+0x80;
        buff[7] = 0x03;
        slen = Make_GetMACPacket(&buff[8]);
        break;
    case WIFI_SENDPKT_STATE_TURN_OFF_RESPONSE://关机应答
        buff[6] = 0x01+0x80;
        buff[7] = 0x00;
        slen = Make_NormalPacket(&buff[8]);
        break;
    case WIFI_SENDPKT_STATE_TURN_ON_RESPONSE://开机应答
        buff[6] = 0x01+0x80;
        buff[7] = 0x01;
        slen = Make_NormalPacket(&buff[8]);
        break;
    case WIFI_SENDPKT_STATE_TURN_ONOFF_RESPONSE://开关机上报 按键、定时、433遥控、A2422
        buff[6] = 0x01+0x80;
        buff[7] = 0x02;
        slen = Make_NormalPacket(&buff[8]);
        break;
    case WIFI_SENDPKT_STATE_ATTRIBUTE_GET_RESPONSE://属性查询回传
        buff[6] = 0x03+0x80;
        buff[7] = 0x00;
        slen = Make_NormalPacket(&buff[8]);
        break;
    case WIFI_SENDPKT_STATE_ATTRIBUTE_SET_RESPONSE://属性设置回传
        buff[6] = 0x03+0x80;
        buff[7] = 0x01;
        slen = Make_NormalPacket(&buff[8]);
        break;
    case WIFI_SENDPKT_STATE_ATTRIBUTE_CHANGE_RESPONSE://属性变化上报
        buff[6] = 0x03+0x80;
        buff[7] = 0x02;
        slen = Make_NormalPacket(&buff[8]);
        break;
    case WIFI_SENDPKT_STATE_ATTRIBUTE_RESET_RESPONSE://属性恢复回传
        buff[6] = 0x03+0x80;
        buff[7] = 0x03;
        slen = Make_NormalPacket(&buff[8]);
        break;
    case WIFI_SENDPKT_STATE_SCHTASK_QUERY_RESPONSE://定时查询回传
        buff[6] = 0x02+0x80;
        buff[7] = 0x00;
        slen = Make_SchTaskPacket(&buff[8]);
        break;
    case WIFI_SENDPKT_STATE_SCHTASK_ADD_RESPONSE://定时新增回传
        buff[6] = 0x02+0x80;
        buff[7] = 0x01;
        slen = Make_SchTaskPacket(&buff[8]);
        break;
    case WIFI_SENDPKT_STATE_SCHTASK_EDIT_RESPONSE://定时修改回传
        buff[6] = 0x02+0x80;
        buff[7] = 0x02;
        slen = Make_SchTaskPacket(&buff[8]);
        break;
    case WIFI_SENDPKT_STATE_SCHTASK_DEL_RESPONSE://定时删除回传
        buff[6] = 0x02+0x80;
        buff[7] = 0x03;
        slen = Make_SchTaskPacket(&buff[8]);
        break;
    case WIFI_SENDPKT_STATE_SCHTASK_CHANGE_RESPONSE://定时变化上报
        buff[6] = 0x02+0x80;
        buff[7] = 0x04;
        slen = Make_SchTaskPacket(&buff[8]);
        break;
    case WIFI_SENDPKT_WIFI_CONTROL_GET_STATE://wifi控制获取状态
        buff[6] = 0x04+0x80;
        buff[7] = 0x00;
        slen = 0;//Make_WifiControlPacket(&buff[8]);
        break;
    case WIFI_SENDPKT_WIFI_CONTROL_CONNECT://wifi控制配网
        buff[6] = 0x04+0x80;
        buff[7] = 0x01;
        slen = 0;//Make_WifiControlPacket(&buff[8]);
        break;
    case WIFI_SENDPKT_WIFI_CONTROL_CLEAR://wifi控制清除绑定
        buff[6] = 0x04+0x80;
        buff[7] = 0x02;
        slen = 0;//Make_WifiControlPacket(&buff[8]);
        break;
    case WIFI_SENDPKT_WIFI_CONTROL_REG://wifi控制预注册
        buff[6] = 0x04+0x80;
        buff[7] = 0x03;
        slen = 0;//Make_WifiControlPacket(&buff[8]);
        break;
    case WIFI_SENDPKT_WIFI_CONTROL_OTA://wifi控制OTA
        buff[6] = 0x04+0x80;
        buff[7] = 0x04;
        slen = 0;//Make_WifiControlPacket(&buff[8]);
        break;
    case WIFI_SENDPKT_WIFI_CONTROL_GETVER://wifi控制OTA
        buff[6] = 0x04+0x80;
        buff[7] = 0x05;
        slen = 0;//Make_WifiControlPacket(&buff[8]);
        break;
    default:
        buff[6] = 0x00+0x80;
        buff[7] = 0x00;
        slen = 0;//Make_WifiControlPacket(&buff[8]);
        break;
    }

    slen += 9;
    buff[4] = slen;//len-L
    buff[5] = slen>>8;//len-H

    //计算累加和
    sum = 0;
    for(i=0;i<slen-1;i++)
    {
        sum += buff[i];
    }
    buff[slen-1] = sum;
    UART3_SendString(buff, slen);
}

//华氏度转摄氏度
int16_t TempFtoC(int16_t temp)
{
    float ttemp=0.0;

    ttemp = ((float)temp-32.0)/1.8;
    return (int16_t)ttemp;
}

//摄氏度转华氏度
int16_t TempCtoF(int16_t temp)
{
    float ttemp=0.0;

    ttemp = ((float)temp*1.8) + 32.0;
    return (int16_t)ttemp;
}

//温度数值x1.8
int16_t TempMul18(int16_t temp)
{
    float ttemp=0.0;

    ttemp = ((float)temp)*1.8;
    return (int16_t)ttemp;
}

//温度数值除以1.8
int16_t TempDiv18(int16_t temp)
{
    float ttemp=0.0;

    ttemp = ((float)temp)/1.8;
    return (int16_t)ttemp;
}

int16_t TempChange(int16_t temp, uint8_t tunit)
{
    float ttemp=0.0;

    if(tunit == TEMPER_UINT_C)
    {
        ttemp = temp;
    }else if(tunit == TEMPER_UINT_F)
    {
        ttemp = ((float)temp*1.8) + 32.0;
    }
    return ttemp;
}

uint16_t AltitudeChange(uint16_t al,uint8_t aunit)
{
    uint16_t a=0;

    if(aunit == ALTITUDE_UINT_M)
    {
        a = al;
    }else if(aunit == ALTITUDE_UINT_FT)
    {
        a = al*33/10;
    }
    return a;
}

//串口3发送数据包
//void UART3_SendPacket(uint16_t len,uint8_t *buf)
//{
//    uint8_t buff[512] = { 0 };
//    uint16_t slen=0,i=0;
//    uint8_t sum=0;
//
//    slen = len + 6;
//    buff[0] = 0xFE;//Head1
//    buff[1] = 0xAA;//Head2
//    buff[2] = 0x00;//序号
//    buff[3] = slen>>8;//len-H
//    buff[4] = slen&0xFF;//len-L
//
//    //复制数据
//    memcpy(&buff[5],buf,len);
//    //计算累加和
//    sum = 0;
//    for(i=0;i<slen-1;i++)
//    {
//        sum += buff[i];
//    }
//    buff[slen-1] = sum;
//    UART3_SendString(buff, slen);
//}
