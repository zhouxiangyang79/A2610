/*
 * uart0.c
 *
 *  Created on: 2024年11月8日
 *      Author: yyz
 */
#include "HAL.h"

//
uint8_t RxBuff[256];
uint8_t buf_size = 0;

#define COM_TX() GPIOB_SetBits(GPIO_Pin_9)
#define COM_RX() GPIOB_ResetBits(GPIO_Pin_9)

void UART1_INIT(void)
{
	/* 配置串口1：先配置IO口模式，再配置串口 */
	GPIOA_SetBits(bTXD1);
	GPIOB_SetBits(GPIO_Pin_9);  //收发使能脚
	GPIOA_SetBits(GPIO_Pin_15); //串口0接收
	GPIOA_ModeCfg(GPIO_Pin_15, GPIO_ModeIN_PU);
	GPIOA_ModeCfg(bRXD1, GPIO_ModeIN_PU); // RXD-配置上拉输入
	GPIOA_ModeCfg(bTXD1, GPIO_ModeOut_PP_20mA); // TXD-配置推挽输出，注意先让IO口输出高电平
	GPIOB_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_20mA);
	UART1_DefInit();
	UART1_BaudRateCfg(4800);
	COM_RX();
	UART1_ByteTrigCfg(UART_1BYTE_TRIG);
	UART1_INTCfg(ENABLE, RB_IER_RECV_RDY | RB_IER_LINE_STAT | RB_IER_THR_EMPTY);
	PFIC_EnableIRQ(UART1_IRQn);
}
/*********************************************************************
 * @fn      UART1_IRQHandler
 *
 * @brief   UART1中断函数
 *
 * @return  none
 */
uint8_t by_flage = 0;
__INTERRUPT
__HIGH_CODE
void UART1_IRQHandler(void)
{
	uint8_t i,j,sum,val;
	uint8_t r[10];
	static uint8_t len=0;

	switch (UART1_GetITFlag())
	{
	case UART_II_LINE_STAT: // 线路状态错误
		val = UART1_GetLinSTA();
		break;
	case UART_II_RECV_RDY: // 数据达到设置触发点

		i = UART1_RecvString(r);
		RxBuff[buf_size] = r[0];
		buf_size++;
		if (RxBuff[0] == 0xAA)
		{
			if (buf_size >= 4)
			{
				if (RxBuff[0] == 0xAA && RxBuff[1] == 0x77) // 接收到帧头
				{
				    len = RxBuff[3];
					// 接收到帧尾
					if (buf_size >= (len+5))
					{
					    sum=0;
					    //进行数据校验
					    for(j=0;j<(len+2);j++)
					    {
					        sum = sum + RxBuff[2+j];
					    }
					    if(sum == RxBuff[len+4])
					    {
					        //通过数据判读是否是至优主机 [14].BIT7 1至优主机带通风、强热
					        if(RxBuff[14]&0x80)
					        {
					            runState.mainBoardType = MAIN_BOARD_TYPE_VENTILATE_FASTHART;
					        }
					        else
					        {
					            runState.mainBoardType = MAIN_BOARD_TYPE_NONE_VENTILATE;
                            }
					        if(RxBuff[2]==0x02)
                            {
                                switch(baseParameters.runMode)
                                {
                                case CTRL_RUNMODE_TEMP:
                                case CTRL_RUNMODE_LEVEL:
                                    if(RxBuff[4]==HEATER_RUNMODE_AUTO)  //主机为自动模式，停止发送开机指令
                                    {
                                        runState.ctrlCmdFlag = 0;
                                    }else {
                                        runState.ctrlCmdFlag = 1;
                                    }
                                    break;
                                default:
                                    runState.ctrlCmdFlag = 0;
                                    break;
                                }
                            }

					        //遥控器存储主机返回数据
                            heaterDatas.runningMode = RxBuff[4];    //运行模式      01自动    02手动   03泵油   20通风
                            heaterDatas.runningStep = RxBuff[5];    //运行步骤
                            heaterDatas.runningLevel = RxBuff[6];   //运行档位
                            heaterDatas.faultType = RxBuff[7];      //故障码
                            heaterDatas.version = RxBuff[9] + RxBuff[8] * 256;  //主机版本号
                            heaterDatas.supplyVoltage = RxBuff[11] + RxBuff[10] * 256;  //电源电压
                            heaterDatas.altitude = RxBuff[13] + RxBuff[12] * 256;   //海拔高度
                            if(heaterDatas.altitude<500)    heaterDatas.altitude = 0;
                            heaterDatas.shellTemp = RxBuff[17] + RxBuff[16] * 256;  //壳体温度
                            heaterDatas.runTime = 0;//RxBuff[21] + RxBuff[20]*256 + RxBuff[19]*256*256 + RxBuff[18]*256*256*256;//运行时间
                            heaterDatas.oilPumpMass = RxBuff[23] + RxBuff[22] * 256;
                            heaterDatas.ignitionVoltage = RxBuff[24];   //点火电压
                            heaterDatas.ignitionPower = RxBuff[25];     //点火功率
                            heaterDatas.ignitionCurrent = RxBuff[26];   //点火电流
                            heaterDatas.windCurrent = RxBuff[29] + RxBuff[28]*256;  //风机转速
					    }
						buf_size = 0;
						len = 0;
						memset(RxBuff, 0, 256);
					}
				}
#if CO_VALUE==1
				else if (RxBuff[0] == 0xAA && RxBuff[1] == 0x88)
                {
				    heaterDatas.sensorType = 0x01;//CO传感器
                    len = RxBuff[3];
                    if (buf_size >= (len+5))
                    {
                        sum=0;
                        //进行数据校验
                        for(j=0;j<(len+2);j++)
                        {
                            sum = sum + RxBuff[2+j];
                        }
                        if(sum == RxBuff[len+4])
                        {
                            //气体精度
                            runState.coAccuracy = RxBuff[10];
                            //偏移
                            runState.coOffset = RxBuff[11];
                            //数值
                            runState.coData = RxBuff[12] + RxBuff[13] * 256;
                            //物理数值
                            if(runState.coAccuracy==0)
                            {
                                runState.coNum = runState.coData+runState.coOffset;
                                runState.coClearCnt = 0;    //超时后 runState.coNum=0;
                            }
                        }
                        buf_size = 0;
                        len = 0;
                        memset(RxBuff, 0, 256);
                    }
                }
#endif
				else if (RxBuff[0] == 0xAA && RxBuff[1] == 0x66)
				{
					by_flage = 1;
					if (buf_size >= 16)
					{
						buf_size = 0;
						memset(RxBuff, 0, 256);
					}
				}
				else
				{
					by_flage = 1;
					buf_size = 0;
					memset(RxBuff, 0, 256);
				}
			}
		}
		else
		{
			by_flage = 1;
			buf_size = 0;
			memset(RxBuff, 0, 256);
		}
		if (buf_size >= 256)
		{
			buf_size = 0;
			memset(RxBuff, 0, 256);
		}
		break;
	case UART_II_RECV_TOUT: // 接收超时，暂时一帧数据接收完成

		break;
	case UART_II_THR_EMPTY: // 发送缓存区空，可继续发送
		COM_RX();
		break;

	case UART_II_MODEM_CHG: // 只支持串口0
		break;

	default:
		break;
	}
}

uint8_t checksum(uint8_t* buff, uint8_t length)
{
	uint8_t sum = 0;
	for (int i = 0; i < length; i++)
	{
		sum = sum + *(buff + i);
	}
	return sum;
}

//开机时获取主句状态，并设置档位
void UART1_GETDATA(void)
{
	uint8_t buff[17] = { 0 };

	runState.usart1DelayCnt = SERIAL_SEND_TIME;
	buff[0] = 0xAA;
	buff[1] = 0x66;
	buff[2] = 0x02;
	buff[3] = 11;
	//在飞运行界面时，始终发送自动温控档位或者手动档位
	if(baseParameters.runMode == CTRL_RUNMODE_TEMP)
	{
	    if(runState.auto_setLevel == 0) runState.auto_setLevel=1;
	    else if(runState.auto_setLevel>POWER_LEVEL_NUM_MAX)  runState.auto_setLevel = POWER_LEVEL_NUM_MAX;
	    buff[8] = runState.auto_setLevel;
	}
	else if(baseParameters.runMode == CTRL_RUNMODE_LEVEL)
    {
	    if(runState.setLevel == 0) runState.setLevel=1;
        else if(runState.setLevel>POWER_LEVEL_NUM_MAX)  runState.setLevel = POWER_LEVEL_NUM_MAX;
	    buff[8] = runState.setLevel;
    }
	if(baseParameters.runMode == CTRL_RUNMODE_VENTILATE)
	{
	    buff[8] = runState.ventilateLevel;
	    buff[9] = 20; //通风模式
	}else
	{
	    if(runState.runModel == REMOTE_STATE_PUMPOIL)
	    {
	        buff[9] = 0x03;
	    }
	    else {
	        buff[9] = 0x06;
        }
    }
    switch(baseParameters.runMode)
    {
    case CTRL_RUNMODE_TEMP:
        if(heaterDatas.runningLevel==11)    buff[2] = 0x06;
        buff[10] = 2;   //0老开关  1自动恒温  2手动档位  3强热
        buff[11] = (uint8_t)GetRunningSetTemper();
        break;
    case CTRL_RUNMODE_LEVEL:
        if(heaterDatas.runningLevel==11)    buff[2] = 0x06;
        buff[10] = 2;   //0老开关  1自动恒温  2手动档位  3强热
        break;
    case CTRL_RUNMODE_FASTHEAT:
        //如果运行档位不为11,
        if(heaterDatas.runningLevel != 11)
        {
            buff[2] = 0x06;
        }
        buff[8] = 5;
        buff[10] = 3;   //0老开关  1自动恒温  2手动档位  3强热
        buff[11] = (uint8_t)GetRunningSetTemper();   //设定温度  8~36
        buff[12] = (int8_t)(GetTemperatureValue()/10);
        break;
    }
    //判断如果主机处于待机状态，修改控制命令
    if(heaterDatas.runningMode==0x04 && heaterDatas.runningStep==0x05)//自动启停待机状态
    {
        if(GetDisSetTemperModeStandby())//自动启停打开
        {
            if(baseParameters.runMode!=CTRL_RUNMODE_TEMP)
            {
                buff[2] = 0x06;
            }
        }
        else//自动启停关闭
        {
            buff[2] = 0x06;
        }
    }
    if(heaterDatas.cabAltitude == 0xFFFF)   buff[13] = 0;
    else                                    buff[13] = heaterDatas.cabAltitude/25;
	buff[15] = checksum(buff + 2, 13);
	COM_TX();

    UART1_SendString(buff, 17);
}

//主机开机
void UART1_CMDON(void)
{
	uint8_t buff[17] = { 0 };

	runState.usart1DelayCnt = SERIAL_SEND_TIME;
	buff[0] = 0xAA;
	buff[1] = 0x66;
	buff[2] = 0x06;
	buff[3] = 11;
	if(baseParameters.runMode == CTRL_RUNMODE_TEMP)
    {
        if(runState.auto_setLevel == 0) runState.auto_setLevel=1;
        else if(runState.auto_setLevel>POWER_LEVEL_NUM_MAX)  runState.auto_setLevel = POWER_LEVEL_NUM_MAX;
        buff[8] = runState.auto_setLevel;
        if(GetDisSetTemperModeStandby())
        {
            buff[10] = 0x02;    //自动恒温
            buff[11] = (uint8_t)GetRunningSetTemper();   //设定温度  8~36
        }
        else
        {
            buff[10] = 0x02;    //手动档位
        }
    }
	else if(baseParameters.runMode == CTRL_RUNMODE_LEVEL)
    {
        if(runState.setLevel == 0) runState.setLevel=1;
        else if(runState.setLevel>POWER_LEVEL_NUM_MAX)  runState.setLevel = POWER_LEVEL_NUM_MAX;
        buff[8] = runState.setLevel;
        if(baseParameters.runMode == CTRL_RUNMODE_LEVEL && runState.mainBoardType == MAIN_BOARD_TYPE_VENTILATE_FASTHART)
        {
            buff[10] = 0x02;
        }
        else buff[10] = 0;
    }
	else if(baseParameters.runMode == CTRL_RUNMODE_FASTHEAT)
    {
        //强热
        buff[8] = 5;
        buff[10] = 0x03;
        buff[11] = (uint8_t)GetRunningSetTemper();   //设定温度  8~36
        buff[12] = (int8_t)(GetTemperatureValue()/10);
    }

	//当前模式判断  开机、通风
	if(baseParameters.runMode == CTRL_RUNMODE_VENTILATE)
    {
	    if(runState.ventilateLevel == 0) runState.ventilateLevel=1;
        else if(runState.ventilateLevel > POWER_LEVEL_NUM_MAX)  runState.ventilateLevel = POWER_LEVEL_NUM_MAX;
        buff[8] = runState.ventilateLevel;
        buff[9] = 20; //通风模式
    }else
    {
        buff[9] = 0x06;
    }
	if(heaterDatas.cabAltitude == 0xFFFF)   buff[13] = 0;
    else                                    buff[13] = heaterDatas.cabAltitude/25;
	buff[15] = checksum(buff + 2, 13);
	COM_TX();
	UART1_SendString(buff, 17);
}

//主机关机
void UART1_CMDOFF(void)
{
	uint8_t buff[17] = { 0 };

	runState.usart1DelayCnt = SERIAL_SEND_TIME;
	buff[0] = 0xAA;
	buff[1] = 0x66;
	buff[2] = 0x06;
	buff[3] = 11;
	if(baseParameters.runMode == CTRL_RUNMODE_TEMP)
    {
        if(runState.auto_setLevel == 0) runState.auto_setLevel=1;
        else if(runState.auto_setLevel>POWER_LEVEL_NUM_MAX)  runState.auto_setLevel = POWER_LEVEL_NUM_MAX;
        buff[8] = runState.auto_setLevel;
    }
    else if(baseParameters.runMode == CTRL_RUNMODE_LEVEL || baseParameters.runMode == CTRL_RUNMODE_VENTILATE)
    {
        if(runState.setLevel == 0) runState.setLevel=1;
        else if(runState.setLevel>POWER_LEVEL_NUM_MAX)  runState.setLevel = POWER_LEVEL_NUM_MAX;
        buff[8] = runState.setLevel;
    }else
    {
        buff[8] = 0;
    }
	buff[9] = 0x05;
	if(heaterDatas.cabAltitude == 0xFFFF)   buff[13] = 0;
    else                                    buff[13] = heaterDatas.cabAltitude/25;
	buff[15] = checksum(buff + 2, 13);
	COM_TX();
	UART1_SendString(buff, 17);
}

//主机泵油
void UART1_CMDPUMPOIL(void)
{
	uint8_t buff[17] = { 0 };

	runState.usart1DelayCnt = SERIAL_SEND_TIME;
	buff[0] = 0xAA;
	buff[1] = 0x66;
	buff[2] = 0x06;
	buff[3] = 11;
	buff[8] = POWER_LEVEL_NUM_MIN;
	buff[9] = 0x03;
	if(heaterDatas.cabAltitude == 0xFFFF)   buff[13] = 0;
    else                                    buff[13] = heaterDatas.cabAltitude/25;
	buff[15] = checksum(buff + 2, 13);
	COM_TX();
	UART1_SendString(buff, 17);
}
