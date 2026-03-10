################################################################################
# MRS Version: 1.9.2
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../APP/RF_PHY.c \
../APP/button.c \
../APP/date_time.c \
../APP/hp203b.c \
../APP/lcd.c \
../APP/multi_button.c \
../APP/multi_timer.c \
../APP/parameter.c \
../APP/peripheral.c \
../APP/peripheral_main.c \
../APP/ring_buffer.c \
../APP/ring_buffer_chapter.c \
../APP/running.c 

OBJS += \
./APP/RF_PHY.o \
./APP/button.o \
./APP/date_time.o \
./APP/hp203b.o \
./APP/lcd.o \
./APP/multi_button.o \
./APP/multi_timer.o \
./APP/parameter.o \
./APP/peripheral.o \
./APP/peripheral_main.o \
./APP/ring_buffer.o \
./APP/ring_buffer_chapter.o \
./APP/running.o 

C_DEPS += \
./APP/RF_PHY.d \
./APP/button.d \
./APP/date_time.d \
./APP/hp203b.d \
./APP/lcd.d \
./APP/multi_button.d \
./APP/multi_timer.d \
./APP/parameter.d \
./APP/peripheral.d \
./APP/peripheral_main.d \
./APP/ring_buffer.d \
./APP/ring_buffer_chapter.d \
./APP/running.d 


# Each subdirectory must supply rules for building sources it contributes
APP/%.o: ../APP/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common  -g -DxDEBUG=0 -DDEVICE_TYPE=0x11220005 -DCLK_OSC32K=0 -DVCC_CHICK=0 -DWWDG_EN=1 -DCH58xBLE_ROM=1 -DLIB_FLASH_BASE_ADDRESSS=0x00040000 -I"../StdPeriphDriver/inc" -I"../RVMSIS" -I../HAL/include -I../LIB -I../Profile/include -I../APP/include -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

