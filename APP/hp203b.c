/*
 * hp203b.c
 *
 *  Created on: 2024年11月20日
 *      Author: yyz
 */

#include "HAL.h"
#include "hp230b.h"

#define HP203B_ADDRESS 0x76

//void I2C_Delay(void)
//{
//    uint8_t i;
//    DelayUs(10);
//}
////================================================
//void I2C_Start(void)
//{
//    SET_SDA_HIGH();
//    I2C_Delay();
//    SET_SCL_HIGH();
//    I2C_Delay();
//
//    SET_SDA_LOW();
//    I2C_Delay();
//    SET_SCL_LOW();
//    I2C_Delay();
//}
////================================================
//void I2C_Stop(void)
//{
//    SET_SCL_LOW();
//    I2C_Delay();
//    SET_SDA_LOW();
//    I2C_Delay();
//    SET_SCL_HIGH();
//    I2C_Delay();
//    SET_SDA_HIGH();
//    I2C_Delay();
//}
//
//
////================================================
//uint8_t I2C_ReadByte(uint8_t ack)//95.24k
//{
//    uint8_t ucValue=0;
//    uint8_t ucIndex;
//
//
//    for ( ucIndex = 0; ucIndex < 8; ucIndex++ )
//    {
//        ucValue <<= 1;
//        SET_SCL_LOW();
//        I2C_Delay();
//        SET_SCL_HIGH();
//        I2C_Delay();
//
//        if(READ_PIN_SDA()!=0)
//        {
//            ucValue |= 1;
//        }
//
//        I2C_Delay();
//        SET_SCL_LOW();
//        //I2C_Delay();
//    }
//
//    SET_SCL_LOW();
////    if(ack)
////    {
////        SET_SDA_LOW();
////    }else
////    {
////        SET_SDA_HIGH();
////    }
//    I2C_Delay();
//    SET_SCL_HIGH();
//    I2C_Delay();
//    SET_SCL_LOW();
////    PIN_SDA_OUT();
//    return ucValue;
//}
//
////================================================
//void I2C_WriteByte( uint8_t ucData )//117.65k
//{
//    uint8_t i;
//    for( i = 0; i < 8; i++ )
//    {
//        SET_SCL_LOW();
//        //I2C_Delay();
//        if((ucData & 0x80) == 0x80)
//        {
//            SET_SDA_HIGH();
//        }
//        else
//        {
//            SET_SDA_LOW();
//        }
//        I2C_Delay();
//
//        SET_SCL_HIGH();
//        I2C_Delay();
//        ucData <<= 1;
//        SET_SCL_LOW();
//        //I2C_Delay();
//    }
//
//    SET_SCL_LOW();
//    SET_SDA_HIGH();
//    I2C_Delay();
//    SET_SCL_HIGH();
//    I2C_Delay();
//    SET_SCL_LOW();
//}
//
//// 写数据
//void I2C_Write(uint8_t deviceAddr, uint8_t *data, uint8_t length) {
//    I2C_Start();
//    I2C_WriteByte(deviceAddr << 1); // 写操作地址
//    for (int i = 0; i < length; i++) {
//        I2C_WriteByte(data[i]);
//    }
//    I2C_Stop();
//}
//
//// 读数据
//void I2C_Read(uint8_t deviceAddr, uint8_t *buffer, uint8_t length) {
//    I2C_Start();
//    I2C_WriteByte((deviceAddr << 1) | 1); // 读操作地址
//    PIN_SDA_IN();
//    for (int i = 0; i < length; i++) {
//        buffer[i] = I2C_ReadByte(i < (length - 1));
//    }
//    PIN_SDA_OUT();
//    I2C_Stop();
//}
//
///****************************************************************************************/
void HP230BIO_Init(void)
{
    GPIOA_SetBits(GPIO_Pin_1 | GPIO_Pin_2);
    GPIOA_ModeCfg(GPIO_Pin_1 | GPIO_Pin_2, GPIO_ModeOut_PP_5mA);
}

unsigned char user_cmd_osr = CMD_OSR_1024;  //防止无初始化操作

//产生IIC起始信号
void IIC_Start(void)
{
    SET_SCL_HIGH();
    mDelayuS(5);
    SET_SDA_HIGH();
    mDelayuS(5);
    SET_SDA_LOW(); //START:when CLK is high,DATA change form high to low
    mDelayuS(5);
    SET_SCL_LOW(); //钳住I2C总线，准备发送或接收数据
    mDelayuS(5);
}
//产生IIC停止信号
void IIC_Stop(void)
{
    SET_SCL_LOW();
    mDelayuS(5);
    SET_SDA_LOW(); //STOP:when CLK is high DATA change form low to high
    mDelayuS(5);
    SET_SCL_HIGH();
    mDelayuS(5);
    SET_SDA_HIGH();    //发送I2C总线结束信号
    mDelayuS(5);
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t IIC_Wait_Ack(void)
{
    uint8_t ucErrTime=0;

    SET_SCL_LOW();
    mDelayuS(5);

    PIN_SDA_IN();

    SET_SCL_HIGH();
    mDelayuS(5);
    while(READ_PIN_SDA()!=0)
    {
        ucErrTime++;
        if(ucErrTime>250)
        {
            IIC_Stop();
//            printf("IIC STOP!\r\n");
            return 1;
        }
    }
    SET_SCL_LOW();  //时钟输出0
    mDelayuS(5);
    PIN_SDA_OUT();
    return 0;
}
//产生ACK应答
void IIC_Ack(void)
{
    SET_SCL_LOW();
    mDelayuS(5);
    SET_SDA_LOW();
    mDelayuS(5);
    SET_SCL_HIGH();
    mDelayuS(5);
    SET_SCL_LOW();
    mDelayuS(5);
}
//不产生ACK应答
void IIC_NAck(void)
{
    SET_SCL_LOW();
    mDelayuS(5);
    SET_SDA_HIGH();
    mDelayuS(5);
    SET_SCL_HIGH();
    mDelayuS(5);
    SET_SCL_LOW();
    mDelayuS(5);
}
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
void IIC_Send_Byte(uint8_t txd)
{
    uint8_t t;

    for(t=0;t<8;t++)
    {
        SET_SCL_LOW(); //拉低时钟开始数据传输
        mDelayuS(5);
        if(txd & 0x80)
            SET_SDA_HIGH();
        else
            SET_SDA_LOW();
        txd <<= 1;
        mDelayuS(5);//对TEA5767这三个延时都是必须的
        SET_SCL_HIGH();
        mDelayuS(5);
    }
    SET_SCL_LOW();
    mDelayuS(5);
}
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK
uint8_t IIC_Read_Byte(uint8_t ack)
{
    uint8_t i,receive=0;
    SET_SCL_LOW();
    mDelayuS(5);
    PIN_SDA_IN();
    //SET_SDA_HIGH();
    for(i=0;i<8;i++ )
    {

        SET_SCL_HIGH();
        mDelayuS(5);
        receive <<= 1;
        if(READ_PIN_SDA()!=0)
            receive |= 0x01;
        mDelayuS(5);
        SET_SCL_LOW();
        mDelayuS(5);
    }
    PIN_SDA_OUT();
    if (!ack)
            IIC_NAck(); //发送nACK
    else
            IIC_Ack();  //发送ACK
    return receive;
}

void HP203X_WriteCmd(uint8_t cmd)
{
    IIC_Start();
    IIC_Send_Byte(HP20X_ADDR);
    IIC_Wait_Ack();
    IIC_Send_Byte(cmd);
    IIC_Wait_Ack();
    IIC_Stop();
    mDelaymS(1);
}

void HP203X_WriteReg(uint8_t reg,uint8_t dat)
{
    IIC_Start();
    IIC_Send_Byte(HP20X_ADDR);
    IIC_Wait_Ack();
    IIC_Send_Byte(reg | HP20X_WR_REG_MODE);
    IIC_Wait_Ack();
    IIC_Send_Byte(dat);
    IIC_Wait_Ack();
    IIC_Stop();
    mDelaymS(1);
}

uint8_t HP203X_ReadReg(uint8_t reg)
{
    uint8_t dat = 0;
    IIC_Start();
    IIC_Send_Byte(HP20X_ADDR);
    IIC_Wait_Ack();
    IIC_Send_Byte(reg | HP20X_RD_REG_MODE);
    IIC_Wait_Ack();
    IIC_Stop();
    IIC_Start();
    IIC_Send_Byte(HP20X_ADDR | 0x01);
    IIC_Wait_Ack();
    dat = IIC_Read_Byte(0);
    IIC_Stop();
    return dat;
}

uint32_t HP203X_Read3Bytes(uint8_t cmd)
{
    uint32_t receive = 0;
    IIC_Start();
    IIC_Send_Byte(HP20X_ADDR);
    IIC_Wait_Ack();
    IIC_Send_Byte(cmd);
    IIC_Wait_Ack();
    IIC_Stop();
    IIC_Start();
    IIC_Send_Byte(HP20X_ADDR | 0x01);
    IIC_Wait_Ack();
    receive |= IIC_Read_Byte(1);
    receive <<= 8;
    receive |= IIC_Read_Byte(1);
    receive <<= 8;
    receive |= IIC_Read_Byte(0);
    IIC_Stop();
    return receive;
}

void HP203X_Read6Bytes(uint8_t cmd,uint8_t *buf)
{
    uint32_t receive = 0;
    IIC_Start();
    IIC_Send_Byte(HP20X_ADDR);
    IIC_Wait_Ack();
    IIC_Send_Byte(cmd);
    IIC_Wait_Ack();
    IIC_Stop();
    IIC_Start();
    IIC_Send_Byte(HP20X_ADDR | 0x01);
    IIC_Wait_Ack();
    buf[0] = IIC_Read_Byte(1);
    buf[1] = IIC_Read_Byte(1);
    buf[2] = IIC_Read_Byte(1);
    buf[3] = IIC_Read_Byte(1);
    buf[4] = IIC_Read_Byte(1);
    buf[5] = IIC_Read_Byte(0);
    IIC_Stop();
}

uint8_t HP203X_RST(void)
{
    static uint8_t cnt=0;
    HP203X_WriteCmd(CMD_SOFT_RST);
    mDelaymS(1);    // > 400us
    while(!(HP203X_ReadReg(REG_INT_SRC) >> REG_INT_SRC_DEVRDY))    //设备就绪
    {
        mDelaymS(1);
        cnt++;
        if(cnt>50)
        {
            cnt = 0;
            return FALSE;
        }
    }
    return TRUE;
}

uint32_t HP203X_ReadTemperature(void)
{
    uint32_t temperature = 0;
    HP203X_WriteCmd(HP20X_SET_OSR | user_cmd_osr | CMD_CH_T);   //启动温度转换
    switch(user_cmd_osr)
    {
        case CMD_OSR_128:
            mDelaymS(Delay_OSR_128);
            break;
        case CMD_OSR_256:
            mDelaymS(Delay_OSR_256);
            break;
        case CMD_OSR_512:
            mDelaymS(Delay_OSR_512);
            break;
        case CMD_OSR_1024:
            mDelaymS(Delay_OSR_1024);
            break;
        case CMD_OSR_2048:
            mDelaymS(Delay_OSR_2048);
            break;
        case CMD_OSR_4096:
            mDelaymS(Delay_OSR_4096);
            break;
        default:
            break;
    }
    temperature = HP203X_Read3Bytes(CMD_READ_T);
    return temperature;
}

uint32_t HP203X_ReadPress(void)
{
    uint32_t press = 0;
    HP203X_WriteCmd(HP20X_SET_OSR | user_cmd_osr | CMD_CH_PT);  //启动气压、温度转换
    switch(user_cmd_osr)
    {
        case CMD_OSR_128:
            mDelaymS(Delay_OSR_128);
            break;
        case CMD_OSR_256:
            mDelaymS(Delay_OSR_256);
            break;
        case CMD_OSR_512:
            mDelaymS(Delay_OSR_512);
            break;
        case CMD_OSR_1024:
            mDelaymS(Delay_OSR_1024);
            break;
        case CMD_OSR_2048:
            mDelaymS(Delay_OSR_2048);
            break;
        case CMD_OSR_4096:
            mDelaymS(Delay_OSR_4096);
            break;
        default:
            break;
    }
    press = HP203X_Read3Bytes(CMD_READ_P);
    return press;
}

uint32_t HP203X_ReadAltimet(void)
{
    uint32_t altimet = 0;
    HP203X_WriteCmd(HP20X_SET_OSR | user_cmd_osr | CMD_CH_PT);  //启动海拔、温度转换
    switch(user_cmd_osr)
    {
        case CMD_OSR_128:
            mDelaymS(Delay_OSR_128);
            break;
        case CMD_OSR_256:
            mDelaymS(Delay_OSR_256);
            break;
        case CMD_OSR_512:
            mDelaymS(Delay_OSR_512);
            break;
        case CMD_OSR_1024:
            mDelaymS(Delay_OSR_1024);
            break;
        case CMD_OSR_2048:
            mDelaymS(Delay_OSR_2048);
            break;
        case CMD_OSR_4096:
            mDelaymS(Delay_OSR_4096);
            break;
        default:
            break;
    }
    altimet = HP203X_Read3Bytes(CMD_READ_A);
    return altimet;
}

void HP203X_StartAltimetAndTemperature(void)
{
    HP203X_WriteCmd(HP20X_SET_OSR | user_cmd_osr | CMD_CH_PT);  //启动海拔、温度转换
}

void HP203X_ReadAltimetAndTemperature(uint32_t *pressure, uint32_t *temperature)
{
    uint8_t dataBuf[6];

    HP203X_Read6Bytes(CMD_READ_AT, dataBuf);
    *pressure = (dataBuf[0] << 16) | (dataBuf[1] << 8) | dataBuf[2];
    *temperature = (dataBuf[3] << 16) | (dataBuf[4] << 8) | dataBuf[5];
}

void get_HP203BAH(void)
{
    static uint8_t state=0;
    int32_t a=0,t=0;
    switch(state)
    {
    case 0:
        if(HP203X_init()==FALSE)
        {
            state = 0;
            heaterDatas.cabAltitude = 0xFFFF;
        }else
        {
            state++;
        }
        break;
    case 1:
        state++;
        break;
    case 2:
        //写海拔偏移 80m  0x1F40=8000
        HP203X_WriteReg(0,0x40);
        HP203X_WriteReg(1,0x1F);
        state++;
        break;
    case 3:
        state++;
        break;
    case 4:
        HP203X_StartAltimetAndTemperature();
        state++;
        break;
    case 5:
        a = HP203X_Read3Bytes(CMD_READ_A);//读取高度
        if(a&0xF00000)
        {
//            a &= 0x0FFFFF;
            PRINT("aaaaaaah:0x%x m\r\n",a);
            a = ~a;
            a &= 0x00FFFFFF;
            a = a+1;
            a = a/100;
            //负数
            PRINT("aaaaaaah:-%u m\r\n",a);
            heaterDatas.cabAltitude = 0;
        }
        else
        {
            PRINT("bbbbbbh:0x%x m\r\n",a);
            a = a/100;
            heaterDatas.cabAltitude = a;  //存储开关采集海拔
            //正数
            PRINT("cccccccch:%u m\r\n",heaterDatas.cabAltitude);
        }
        state = 0;
        break;
    default:
        state = 0;
        break;
    }
}

uint8_t HP203X_init(void)
{
    mDelaymS(1);    // > 400us
    user_cmd_osr = CMD_OSR_4096;    //可以改成自己喜欢的
    return  (HP203X_RST());
}
