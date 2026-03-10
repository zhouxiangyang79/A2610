/********************************** (C) COPYRIGHT *******************************
 * File Name          : wifi_a2610.c
 * Description        : A2610 WiFi 模块 (ESP8266)
 *                    - UART1: PA4(TX), PA5(RX)
 *                    - RST: PA6
 *                    - 基于 A2200BS 工程添加
 ********************************************************************************/

#include "HAL.h"
#include "wifi_a2610.h"
#include "running.h"
#include "parameter.h"
#include "lcd.h"

/* WiFi 状态 */
WiFiState_t g_wifiState = WIFI_STATE_OFF;
uint8_t g_wifiConnected = 0;

/* 接收缓冲区 */
#define WIFI_RX_BUF_SIZE    256
static uint8_t wifiRxBuf[WIFI_RX_BUF_SIZE];
static volatile uint16_t wifiRxHead = 0;
static volatile uint16_t wifiRxTail = 0;

/* 协议解析状态 */
typedef enum {
    PROTO_STATE_IDLE = 0,
    PROTO_STATE_HEADER1,
    PROTO_STATE_HEADER2,
    PROTO_STATE_CMD,
    PROTO_STATE_LEN,
    PROTO_STATE_DATA,
    PROTO_STATE_CRC
} ProtoState_t;

static ProtoState_t protoState = PROTO_STATE_IDLE;
static uint8_t protoCmd = 0;
static uint8_t protoLen = 0;
static uint8_t protoData[64];
static uint8_t protoIndex = 0;

/* 定时器 */
static uint32_t wifiTick = 0;
static uint32_t lastHeartbeat = 0;

/*********************************************************************
 * @fn      WiFi_Init
 *
 * @brief   WiFi 初始化
 *
 * @return  none
 */
void WiFi_Init(void)
{
    // 配置 WiFi 复位引脚 PA6
    GPIOA_ModeCfg(GPIO_Pin_6, GPIO_ModeOut_PP_5mA);
    GPIOA_SetBits(GPIO_Pin_6);
    
    // 配置 UART1: PA4(TX), PA5(RX)
    GPIOA_ModeCfg(GPIO_Pin_4, GPIO_ModeOut_PP_5mA);  // TX
    GPIOA_ModeCfg(GPIO_Pin_5, GPIO_ModeIN_PU);       // RX
    
    UART1_DefInit();
    UART1_BaudRateCfg(115200);
    
    // 使能接收中断
    UART1_INTCfg(ENABLE, RB_IER_RECV_RDY);
    PFIC_EnableIRQ(UART1_IRQn);
    
    // 清空缓冲区
    wifiRxHead = 0;
    wifiRxTail = 0;
    
    g_wifiState = WIFI_STATE_INIT;
    g_wifiConnected = 0;
    
    PRINT("WiFi Init OK\n");
}

/*********************************************************************
 * @fn      WiFi_Reset
 *
 * @brief   WiFi 复位
 *
 * @return  none
 */
void WiFi_Reset(void)
{
    GPIOA_ResetBits(GPIO_Pin_6);
    DelayMs(200);
    GPIOA_SetBits(GPIO_Pin_6);
    DelayMs(500);
}

/*********************************************************************
 * @fn      WiFi_Process
 *
 * @brief   WiFi 处理循环 (在 TMOS 任务中调用)
 *
 * @return  none
 */
void WiFi_Process(void)
{
    static uint32_t stateTick = 0;
    uint32_t now = sys_get_tick();
    
    // 处理接收数据
    WiFi_ProcessRxData();
    
    // 状态机
    switch(g_wifiState)
    {
        case WIFI_STATE_INIT:
            if(now - stateTick > 1000)
            {
                stateTick = now;
                WiFi_Reset();
                g_wifiState = WIFI_STATE_RESET;
            }
            break;
            
        case WIFI_STATE_RESET:
            if(now - stateTick > 2000)
            {
                stateTick = now;
                WiFi_SendAT("AT\r\n");
            }
            if(WiFi_CheckResponse("OK", 1000))
            {
                PRINT("WiFi AT OK\n");
                g_wifiState = WIFI_STATE_CONFIG;
                stateTick = now;
            }
            break;
            
        case WIFI_STATE_CONFIG:
            if(now - stateTick > 500)
            {
                stateTick = now;
                WiFi_SendAT("AT+CWMODE=1\r\n");
                DelayMs(100);
                WiFi_SendAT("AT+CWAUTOCONN=1\r\n");
                DelayMs(100);
                g_wifiState = WIFI_STATE_CONNECTING;
            }
            break;
            
        case WIFI_STATE_CONNECTING:
            if(now - stateTick > 3000)
            {
                stateTick = now;
                WiFi_SendAT("AT+CIPSTATUS\r\n");
            }
            if(WiFi_CheckResponse("STATUS:2", 500) || 
               WiFi_CheckResponse("STATUS:3", 500) ||
               WiFi_CheckResponse("STATUS:4", 500))
            {
                PRINT("WiFi Connected!\n");
                g_wifiConnected = 1;
                g_wifiState = WIFI_STATE_CONNECTED;
            }
            break;
            
        case WIFI_STATE_CONNECTED:
            if(now - lastHeartbeat > 30000)
            {
                lastHeartbeat = now;
                WiFi_SendHeartbeat();
            }
            if(now - wifiTick > 5000)
            {
                wifiTick = now;
                WiFi_SendStatus();
            }
            break;
            
        case WIFI_STATE_ERROR:
            if(now - stateTick > 10000)
            {
                stateTick = now;
                g_wifiState = WIFI_STATE_INIT;
            }
            break;
            
        default:
            break;
    }
}

/*********************************************************************
 * @fn      WiFi_SendAT
 *
 * @brief   发送 AT 指令
 *
 * @param   cmd - AT 指令
 *
 * @return  none
 */
void WiFi_SendAT(char* cmd)
{
    while(*cmd)
    {
        UART1_SendByte(*cmd++);
    }
}

/*********************************************************************
 * @fn      WiFi_SendData
 *
 * @brief   发送数据
 *
 * @param   data - 数据
 * @param   len - 长度
 *
 * @return  none
 */
void WiFi_SendData(uint8_t* data, uint8_t len)
{
    if(!g_wifiConnected) return;
    
    char cmd[32];
    sprintf(cmd, "AT+CIPSEND=%d\r\n", len);
    WiFi_SendAT(cmd);
    DelayMs(50);
    
    for(uint8_t i = 0; i < len; i++)
    {
        UART1_SendByte(data[i]);
    }
}

/*********************************************************************
 * @fn      WiFi_CheckResponse
 *
 * @brief   检查响应
 *
 * @param   resp - 期望响应
 * @param   timeout - 超时时间
 *
 * @return  0: 未收到, 1: 收到
 */
uint8_t WiFi_CheckResponse(char* resp, uint32_t timeout)
{
    uint32_t start = sys_get_tick();
    char rxBuf[64];
    uint8_t rxIndex = 0;
    
    while(sys_get_tick() - start < timeout)
    {
        uint8_t data;
        while(WiFi_ReadByte(&data))
        {
            if(data == '\n' || rxIndex >= 63)
            {
                rxBuf[rxIndex] = '\0';
                if(strstr(rxBuf, resp) != NULL)
                {
                    return 1;
                }
                rxIndex = 0;
            }
            else
            {
                rxBuf[rxIndex++] = data;
            }
        }
    }
    return 0;
}

/*********************************************************************
 * @fn      WiFi_ReadByte
 *
 * @brief   读取单字节
 *
 * @param   data - 数据指针
 *
 * @return  0: 无数据, 1: 有数据
 */
uint8_t WiFi_ReadByte(uint8_t* data)
{
    if(wifiRxHead == wifiRxTail)
    {
        return 0;
    }
    
    *data = wifiRxBuf[wifiRxTail];
    wifiRxTail = (wifiRxTail + 1) % WIFI_RX_BUF_SIZE;
    return 1;
}

/*********************************************************************
 * @fn      WiFi_ProcessRxData
 *
 * @brief   处理接收数据
 *
 * @return  none
 */
void WiFi_ProcessRxData(void)
{
    uint8_t data;
    
    while(WiFi_ReadByte(&data))
    {
        switch(protoState)
        {
            case PROTO_STATE_IDLE:
                if(data == WIFI_PROTO_HEADER1)
                    protoState = PROTO_STATE_HEADER1;
                break;
                
            case PROTO_STATE_HEADER1:
                if(data == WIFI_PROTO_HEADER2)
                    protoState = PROTO_STATE_HEADER2;
                else
                    protoState = PROTO_STATE_IDLE;
                break;
                
            case PROTO_STATE_HEADER2:
                protoCmd = data;
                protoState = PROTO_STATE_CMD;
                break;
                
            case PROTO_STATE_CMD:
                protoLen = data;
                protoIndex = 0;
                if(protoLen == 0)
                    protoState = PROTO_STATE_CRC;
                else if(protoLen <= 64)
                    protoState = PROTO_STATE_DATA;
                else
                    protoState = PROTO_STATE_IDLE;
                break;
                
            case PROTO_STATE_DATA:
                protoData[protoIndex++] = data;
                if(protoIndex >= protoLen)
                    protoState = PROTO_STATE_CRC;
                break;
                
            case PROTO_STATE_CRC:
                if(WiFi_VerifyCRC(protoCmd, protoLen, protoData, data))
                {
                    WiFi_HandleCommand(protoCmd, protoData, protoLen);
                }
                protoState = PROTO_STATE_IDLE;
                break;
                
            default:
                protoState = PROTO_STATE_IDLE;
                break;
        }
    }
}

/*********************************************************************
 * @fn      WiFi_HandleCommand
 *
 * @brief   处理命令
 *
 * @param   cmd - 命令码
 * @param   data - 数据
 * @param   len - 长度
 *
 * @return  none
 */
void WiFi_HandleCommand(uint8_t cmd, uint8_t* data, uint8_t len)
{
    switch(cmd)
    {
        case WIFI_CMD_POWER:
            if(len >= 1)
            {
                if(data[0])
                    TurnOn_Device(baseParameters.runMode);
                else
                    TurnOff_Device();
            }
            break;
            
        case WIFI_CMD_SET_TEMP:
            if(len >= 2)
            {
                int16_t temp = (data[0] << 8) | data[1];
                runState.setTemper = temp;
                Control_RunTemp();
            }
            break;
            
        case WIFI_CMD_SET_LEVEL:
            if(len >= 1)
                SetRunningSetLevel(data[0]);
            break;
            
        case WIFI_CMD_SET_MODE:
            if(len >= 1)
                ControlModel_Change(data[0]);
            break;
            
        case WIFI_CMD_QUERY_STATUS:
            WiFi_SendStatus();
            break;
            
        default:
            break;
    }
}

/*********************************************************************
 * @fn      WiFi_SendStatus
 *
 * @brief   发送状态
 *
 * @return  none
 */
void WiFi_SendStatus(void)
{
    uint8_t buf[32];
    uint8_t len = 0;
    
    buf[len++] = WIFI_PROTO_HEADER1;
    buf[len++] = WIFI_PROTO_HEADER2;
    buf[len++] = WIFI_CMD_REPORT_STATUS;
    buf[len++] = 10;
    
    buf[len++] = (runState.runModel != REMOTE_STATE_POWER_OFF) ? 1 : 0;
    buf[len++] = baseParameters.runMode;
    buf[len++] = GetRunningSetLevel();
    buf[len++] = (runState.setTemper >> 8) & 0xFF;
    buf[len++] = runState.setTemper & 0xFF;
    buf[len++] = (heaterDatas.cabTemp >> 8) & 0xFF;
    buf[len++] = heaterDatas.cabTemp & 0xFF;
    buf[len++] = heaterDatas.faultType;
    buf[len++] = 0;
    
    buf[len++] = WiFi_CalcCRC(&buf[2], len - 3);
    
    WiFi_SendData(buf, len);
}

/*********************************************************************
 * @fn      WiFi_SendHeartbeat
 *
 * @brief   发送心跳
 *
 * @return  none
 */
void WiFi_SendHeartbeat(void)
{
    uint8_t buf[8];
    uint8_t len = 0;
    
    buf[len++] = WIFI_PROTO_HEADER1;
    buf[len++] = WIFI_PROTO_HEADER2;
    buf[len++] = WIFI_CMD_HEARTBEAT;
    buf[len++] = 1;
    buf[len++] = 0x55;
    buf[len++] = WiFi_CalcCRC(&buf[2], 3);
    
    WiFi_SendData(buf, len);
}

/*********************************************************************
 * @fn      WiFi_CalcCRC
 *
 * @brief   计算 CRC
 *
 * @param   data - 数据
 * @param   len - 长度
 *
 * @return  CRC 值
 */
uint8_t WiFi_CalcCRC(uint8_t* data, uint8_t len)
{
    uint8_t crc = 0;
    for(uint8_t i = 0; i < len; i++)
    {
        crc ^= data[i];
        for(uint8_t j = 0; j < 8; j++)
        {
            if(crc & 0x80)
                crc = (crc << 1) ^ 0x31;
            else
                crc <<= 1;
        }
    }
    return crc;
}

/*********************************************************************
 * @fn      WiFi_VerifyCRC
 *
 * @brief   验证 CRC
 *
 * @return  0: 错误, 1: 正确
 */
uint8_t WiFi_VerifyCRC(uint8_t cmd, uint8_t len, uint8_t* data, uint8_t crc)
{
    uint8_t buf[66];
    buf[0] = cmd;
    buf[1] = len;
    memcpy(&buf[2], data, len);
    return (WiFi_CalcCRC(buf, len + 2) == crc);
}

/*********************************************************************
 * @fn      UART1_IRQHandler
 *
 * @brief   UART1 中断处理
 *
 * @return  none
 */
__attribute__((interrupt("WCH-Interrupt-fast")))
void UART1_IRQHandler(void)
{
    if(UART1_GetITFlag(UART1_IT_FLAG_RX_FIFO_RDY))
    {
        while(UART1_GetFlagStatus(UART1_FLAG_RX_FIFO_RDY))
        {
            uint8_t data = UART1_ReceiveByte();
            uint16_t next = (wifiRxHead + 1) % WIFI_RX_BUF_SIZE;
            if(next != wifiRxTail)
            {
                wifiRxBuf[wifiRxHead] = data;
                wifiRxHead = next;
            }
        }
    }
}
