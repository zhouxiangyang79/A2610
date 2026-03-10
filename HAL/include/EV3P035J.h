/*
 * EV3P035J.h
 *
 *  Created on: 2023年6月16日
 *      Author: wx030
 */

#ifndef INCLUDE_EV3P035J_H_
#define INCLUDE_EV3P035J_H_
#include <stdint.h>
#include "device_type.h"
void vEV3P035JInit(void);
void vEV3P035JPlay(uint8_t num);
void vEV3P035JTask(void);

typedef enum
{
    MUTE=1,
#if GERMAN==0 || GERMAN==1
    START_HEAT,     //开始加热
    STARTUP_FAIL,   //启动失败
    LESS_OIL,       //缺油熄火
    PUMP_DIS,       //油泵断路
    IGNITION_FAIL,  //预热塞异常
    IN_TEP_SENSOR,  //进风传感器
    OVER_TEP_SENSOR,//高温传感器
    FAN_MOTOR,      //风机
    OVER_PROTECT,   //高温保护
    VOLTAGE_FAILE,  //电压异常
    STOP_HEART,     //停止加热
    THANKS,         //关机祝福语
    LEVEL_MODE=38,  //档位模式
    TEMP_MODE=39,   //温控模式
    TURN_ON_VOICE=44,       //开机提示
    TURN_OFF_VOICE=45,      //关机提示
    ERROR_VOICE=46,         //故障声
    BE_VOICE=47,            //BE声
    WATER_SENSOR_FAIL=48,   //水温传感器异常
    FIRE_SENSOR_FAIL=49,    //火焰传感器异常
    HEAT_COMPLETE=50,       //加热完成
    VOICE_MAX=57,
#elif GERMAN==2
    TURN_ON_VOICE,      //开机提示
    START_HEAT,         //开始加热
    TEMP_ADD,           //温度增加
    TEMP_DEC,           //温度减少
    LEVEL_ADD,          //档位增加
    LEVEL_DEC,          //档位减少
    TEMP_MODE,          //温控模式
    LEVEL_MODE,         //档位模式
    PUMP_MODE,          //手动泵油
    REMOTE_ADD,         //添加遥控
    STARTUP_FAIL,       //启动失败
    LESS_OIL,           //缺油熄火
    PUMP_DIS,           //油泵断路
    IGNITION_FAIL,      //预热塞异常
    IN_TEP_SENSOR,      //进风传感器
    OVER_TEP_SENSOR,    //高温传感器
    FAN_MOTOR,          //风机
    OVER_PROTECT,       //高温保护
    VOLTAGE_FAILE,      //电压异常
    STOP_HEART,         //停止加热
    STOPPING_HEART,     //正在关闭
    THANKS,             //关机祝福语
    BE_VOICE=1,           //BE声
    VOICE_MAX=24,
#endif
} VOICE_STATE;

#endif /* INCLUDE_EV3P035J_H_ */
