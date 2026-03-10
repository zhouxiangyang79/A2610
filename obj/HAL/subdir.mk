################################################################################
# MRS Version: 1.9.2
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HAL/EV3P035J.c \
../HAL/MCU.c \
../HAL/RTC.c \
../HAL/SLEEP.c \
../HAL/cmt2220ls.c \
../HAL/temp.c \
../HAL/tm1628a.c \
../HAL/uart1.c 

OBJS += \
./HAL/EV3P035J.o \
./HAL/MCU.o \
./HAL/RTC.o \
./HAL/SLEEP.o \
./HAL/cmt2220ls.o \
./HAL/temp.o \
./HAL/tm1628a.o \
./HAL/uart1.o 

C_DEPS += \
./HAL/EV3P035J.d \
./HAL/MCU.d \
./HAL/RTC.d \
./HAL/SLEEP.d \
./HAL/cmt2220ls.d \
./HAL/temp.d \
./HAL/tm1628a.d \
./HAL/uart1.d 


# Each subdirectory must supply rules for building sources it contributes
HAL/%.o: ../HAL/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common  -g -DxDEBUG=0 -DDEVICE_TYPE=0x11220005 -DCLK_OSC32K=0 -DVCC_CHICK=0 -DWWDG_EN=1 -DCH58xBLE_ROM=1 -DLIB_FLASH_BASE_ADDRESSS=0x00040000 -I"../StdPeriphDriver/inc" -I"../RVMSIS" -I../HAL/include -I../LIB -I../Profile/include -I../APP/include -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

