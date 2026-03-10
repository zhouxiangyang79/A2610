/*
 * cmt2220ls.c
 *
 *  Created on: 2026年1月29日
 *      Author: yyz
 */
#include "HAL.h"

uint16_t t_high = 0, t_low = 0, gpio_sta = 0, getdata = 0;
uint8_t rx_index = 0;
uint8_t rfGetOk=0;
uint32_t rxlong = 0;
uint8_t rx[10], rev_buf[10];
uint32_t ykq_key = 0xFFFFFFFF;
uint32_t ykq_key_last;
void vHAL_CMT2220INIT(void)
{
	GPIOB_SetBits(GPIO_Pin_13);
	GPIOB_ModeCfg(GPIO_Pin_13, GPIO_ModeIN_Floating);
	TMR1_TimerInit(FREQ_SYS / 100000);
	TMR1_ITCfg(ENABLE, TMR0_3_IT_CYC_END);
	PFIC_EnableIRQ(TMR1_IRQn);
}
//解码函数
// 50us 对应4k,容差1K,,,
//其他速率请 按比例调整定时器时间 比如8k对应20us，2k对应100us等，40k对应5us，速率高请保证单片机速率够快，
//getdata==1时候才是真正的数据接收到，其他非0都是中间状态
//当getdata==1就能提取数据了，取出完毕应当尽快的清零，否则会阻塞住
void ev1527decode(void) //
{
	if (gpio_sta == 0)
	{
		if (GPIOB_ReadPortPin(GPIO_Pin_13) != 0)       // 如果高就一直循环
		{
			t_high++;      //高时间计时
			if (t_high > 300)         //超时处理
			{
				t_high = 301;   //即使超时也不退出但是不再增加，防止溢出
			}
		}
		else
		{
			gpio_sta = 1; //捉到了低电平信号，有可能收到了一个启始信号,进入低电平计数
			t_low = 1;
		}
	}
	if (gpio_sta == 1)
	{
		if (GPIOB_ReadPortPin(GPIO_Pin_13) == 0)  //低计时
		{
			t_low++;
			if (t_low > 300)  //此处大于22是因为1527启始码的低电平很长，
			{
				t_low = 301;
				switch (getdata)
				//接收到的状态处理，1527如果接受到24个bit后，再次接受到符合1527的特征的bit就丢弃
				{
				case 2:
					getdata = 1;
					break;
				case 3:
					getdata = 0;
					break;
				default:
					break;
				}
			}
		}
		else
		{
			gpio_sta = 2;
		}
	}

	if (gpio_sta == 2)
	{
		if (t_low > 300) 			//超时
			t_low = 0;    		//超时与时间不够是一样的处理，反正都是错误
		if (t_high > 300)			//同上
			t_high = 0;
		if ((t_high > 10 && t_low > 35) || (t_high > 35 && t_low > 10))
		{
			if (t_high > t_low)        //高电平时间大于低电平时间
			{
				//rx[(rx_index >> 3)] |= 0x01 << (rx_index & 0x07); //存储当前bit于数组，，一共24bit   0-23，，所以保存字节顺序是 0,1,2，，bit顺序是从低到高存
			    rx[(rx_index >> 3)] <<=1;
			    rx[(rx_index >> 3)] |= 0x01;
			}
			else
			{
			    rx[(rx_index >> 3)] <<=1;
			    rx[(rx_index >> 3)] &= 0xFE;
				//rx[(rx_index >> 3)] &= ~(0x01 << (rx_index & 0x07));
			}
			if (rx_index > 30) //如果存储了24个bit，这里就是23，那大于22就行了，说明接收成功
			{
				if (!getdata)
				{
					rev_buf[0] = rx[0];
					rev_buf[1] = rx[1];
					rev_buf[2] = rx[2];
					rev_buf[3] = rx[3];
					getdata = 2;
					rfGetOk = 1;
				}
				else
				{
					getdata = 3;
				}
				rx_index = 0;
			}
			rx_index++;
			//接收的bit的索引加1，，高低电平持续时间 清零
		}
		else //时间不符合的处理
		{
			rx_index = 0;  //索引为0
		}

		gpio_sta = 0;
		t_high = 1;
		t_low = 0;
	}
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
void TMR1_IRQHandler(void) // TMR0 定时中断
{
	if (TMR1_GetITFlag(TMR0_3_IT_CYC_END))
	{
		//TMR1_ClearITFlag(TMR0_3_IT_CYC_END); // 清除中断标志
		ev1527decode();
		TMR1_ClearITFlag(TMR0_3_IT_CYC_END); // 清除中断标志
	}
}
