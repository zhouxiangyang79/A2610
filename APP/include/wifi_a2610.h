#ifndef WIFI_A2610_H
#define WIFI_A2610_H

#include "CH58x_common.h"

/* WiFi 状态 */
typedef enum {
    WIFI_STATE_OFF = 0,
    WIFI_STATE_INIT,
    WIFI_STATE_RESET,
    WIFI_STATE_CONFIG,
    WIFI_STATE_CONNECTING,
    WIFI_STATE_CONNECTED,
    WIFI_STATE_ERROR
} WiFiState_t;

/* 协议命令 */
#define WIFI_PROTO_HEADER1      0x55
#define WIFI_PROTO_HEADER2      0xAA

#define WIFI_CMD_POWER          0x01
#define WIFI_CMD_SET_TEMP       0x02
#define WIFI_CMD_SET_LEVEL      0x03
#define WIFI_CMD_SET_MODE       0x04
#define WIFI_CMD_QUERY_STATUS   0x05
#define WIFI_CMD_REPORT_STATUS  0x10
#define WIFI_CMD_HEARTBEAT      0x20

/* A2610 硬件引脚定义 - UART3: PA4(TX), PA5(RX) */
#define WIFI_UART               UART3
#define WIFI_UART_IRQn          UART3_IRQn
#define WIFI_BAUDRATE           115200

#define WIFI_RST_PIN            GPIO_Pin_6   // PA6
#define WIFI_TX_PIN             GPIO_Pin_4   // PA4
#define WIFI_RX_PIN             GPIO_Pin_5   // PA5

/* 外部变量 */
extern WiFiState_t g_wifiState;
extern uint8_t g_wifiConnected;

/* 函数声明 */
void WiFi_Init(void);
void WiFi_Reset(void);
void WiFi_Process(void);
void WiFi_UART_Config(void);

void WiFi_SendAT(char* cmd);
void WiFi_SendData(uint8_t* data, uint8_t len);

uint8_t WiFi_ReadByte(uint8_t* data);
void WiFi_ProcessRxData(void);

uint8_t WiFi_CheckResponse(char* resp, uint32_t timeout);
void WiFi_HandleCommand(uint8_t cmd, uint8_t* data, uint8_t len);

void WiFi_SendStatus(void);
void WiFi_SendHeartbeat(void);

uint8_t WiFi_CalcCRC(uint8_t* data, uint8_t len);
uint8_t WiFi_VerifyCRC(uint8_t cmd, uint8_t len, uint8_t* data, uint8_t crc);

/* 中断处理 */
void UART3_IRQHandler(void);

#endif /* WIFI_A2610_H */
