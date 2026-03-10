#ifndef INCLUDE_DEVICE_TYPE_H
#define INCLUDE_DEVICE_TYPE_H

#ifndef DEVICE_TYPE
#error DEVICE_TYPE Undefine
#endif
#if     DEVICE_TYPE==0x11220005     //A2200BSH
#define CO_VALUE    0
#define EN02        0
#define VERSION     3
#define GERMAN      1
#elif   DEVICE_TYPE==0x11220006     //A2200BSH1
#define CO_VALUE    0
#define EN02        0
#define VERSION     3
#define GERMAN      1
#elif   DEVICE_TYPE==0x11220007     //A2200BSH1E  电动工具款
#define CO_VALUE    0
#define EN02        1
#define VERSION     53
#define GERMAN      1
#elif   DEVICE_TYPE==0x11220008     //A2200BSH1C  支持A2429 CO传感器
#define CO_VALUE    1
#define EN02        0
#define VERSION     102
#define GERMAN      2   //1英德俄    2土耳其
#endif

#endif /* INCLUDE_BUTTON_H_ */
