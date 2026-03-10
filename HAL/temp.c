/*
 * temp.c
 *
 *  Created on: 2023年6月20日
 *      Author: wx030
 */

#include "HAL.h"
#include "math.h"
#define SUPPORT_VOLTAGE 3.3f // 供电电压
#define REFERENCE_VOLTAGE 3.3f // 参考电压
#define DIVIDE 4096 // ADC位数
#define PULL_UP_RES 10 // 上拉电阻
#define TEMP_NUM 19 // 温度总共的档位数

// // 第一行代表温度，分割档位可根据需要随意设置
// // 第二行代表相应的热敏电阻的阻值
// const float temp_table[2][TEMP_NUM] = {
//     { -30, -20, -10, 0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 60, 70, 80, 90, 100 },
//     { 122.0f, 72.04f, 44.09f, 27.86f, 22.39f, 18.13f, 14.77f, 12.12f, 10.0f, 8.3f, 6.92f, 5.81f, 4.89f, 4.14f, 3.01f, 2.23f, 1.67f, 1.27f, 0.98f }
// };

// // 根据二维数组表计算温度
// int Temp_Cnt(int temp)
// {
//     float resist = 0; // 电阻值
//     float voltage = 0; // 电压值
//     float k = 0; // k值
//     float b = 0; // b值

//     // 计算电压值
//     voltage = (float)temp / DIVIDE * REFERENCE_VOLTAGE;
//     // 电压限幅
//     voltage = LIMIT(voltage, 0, SUPPORT_VOLTAGE - 0.01f);
//     // 计算电阻值
//     resist = (PULL_UP_RES * voltage) / (SUPPORT_VOLTAGE - voltage);

//     // 数组之外的数据
//     if (resist <= temp_table[1][TEMP_NUM - 1]) // 电阻值小于最小值，返回值100度
//         return (int)(1000);
//     else if (resist >= temp_table[1][0]) // 电阻值大于最大值，返回值-30度
//         return (int)(-300);
//     else {
//         // 遍历二维数组
//         for (uint8_t i = 0; i < TEMP_NUM - 1; i++) {
//             // 判定是否在计算范围内
//             if (Isinside(resist, temp_table[1][i + 1], temp_table[1][i])) {
//                 // 计算k值与b值
//                 k = (temp_table[0][i] - temp_table[0][i + 1]) / (temp_table[1][i] - temp_table[1][i + 1]);
//                 b = temp_table[0][i] - k * temp_table[1][i];
//                 // 计算出来的是温度与热敏电阻的二元一次函数的k值与b值
//                 break;
//             }
//         }
//         // 返回温度值
//         return (int)((k * resist + b) * 10);
//     }
// }

void AdcInit(void)
{
    GPIOA_ModeCfg(GPIO_Pin_7, GPIO_ModeIN_Floating); // 选择adc通道11做采样，对应 PA7(AIN11)
}

__HIGH_CODE
double AdcPolling(void)
{
    signed short RoughCalib_Value = 0; // ADC粗调偏差值
    //uint16_t result;
    double res,result;

    ADC_ExtSingleChSampInit(SampleFreq_3_2, ADC_PGA_0);
    RoughCalib_Value = ADC_DataCalib_Rough(); // 用于计算ADC内部偏差，记录到全局变量 RoughCalib_Value中
    ADC_ChannelCfg(11);
    result = ADC_ExcutSingleConver() + RoughCalib_Value;
    res = result * 1.05f / (2048 * 1);
    if (result > 4000)
    {
        ADC_ExtSingleChSampInit(SampleFreq_3_2, ADC_PGA_1_2);
        ADC_ChannelCfg(11);
        result = ADC_ExcutSingleConver() + RoughCalib_Value;
        res = ((result / 1024.0) - 1.0) * 1.05f;
        if (result > 4000)
        {
            ADC_ExtSingleChSampInit(SampleFreq_3_2, ADC_PGA_1_4);
            ADC_ChannelCfg(11);
            result = ADC_ExcutSingleConver() + RoughCalib_Value;
            res = ((result / 512.0) - 3.0) * 1.05f;
        }
    }
    return res;
}

__HIGH_CODE
uint16_t VccPolling(void)
{
    signed short RoughCalib_Value = 0; // ADC粗调偏差值
    uint16_t result;
//    double res,result;

    ADC_ExtSingleChSampInit(SampleFreq_8, ADC_PGA_1_4);
    RoughCalib_Value = ADC_DataCalib_Rough(); // 用于计算ADC内部偏差，记录到全局变量 RoughCalib_Value中
    ADC_ChannelCfg(CH_INTE_VBAT);
    result = ADC_ExcutSingleConver() + RoughCalib_Value;
    return result;
}

//获取热敏感电阻温度
__HIGH_CODE
void get_CabTemp(void)
{
    int32_t ts = 0;
    static uint8_t t_cnt = 0;
    static double t=0;
    uint16_t dat = 0;
    t_cnt++;
    t = t + AdcPolling();
    dat = VccPolling();
    if(dat>runState.vccAdcMax) runState.vccAdcMax = dat;
    if(dat<runState.vccAdcMin) runState.vccAdcMin = dat;
    if (t_cnt == 5)
    {
        t = t / 5.0;
//         PRINT("2 %0.2f \n", t);
        t = 10000 * t / (3.3 - t);
        // if (t >= 5000) {
        //     t = 1 / (log(t / 5000) / 3470 + 1 / (273.15 + 25)) - 273.15;
        // } else {
        t = 1.0 / (log(t / 5000) / 3470 + 1.0 / (273.15 + 25)) - 273.15 - 4.0;
        // }
        ts = t * 10;
        ts = ts / 10 * 10;
//         PRINT("%d\n", ts);
        heaterDatas.cabTemp = ts + baseParameters.temperSensorOffset * 10;
        t = 0;
        t_cnt = 0;
    }
}
