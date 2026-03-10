################################################################################
# MRS Version: 1.9.2
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Profile/devinfoservice.c \
../Profile/gattprofile.c 

OBJS += \
./Profile/devinfoservice.o \
./Profile/gattprofile.o 

C_DEPS += \
./Profile/devinfoservice.d \
./Profile/gattprofile.d 


# Each subdirectory must supply rules for building sources it contributes
Profile/%.o: ../Profile/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common  -g -DxDEBUG=0 -DDEVICE_TYPE=0x11220005 -DCLK_OSC32K=0 -DVCC_CHICK=0 -DWWDG_EN=1 -DCH58xBLE_ROM=1 -DLIB_FLASH_BASE_ADDRESSS=0x00040000 -I"../StdPeriphDriver/inc" -I"../RVMSIS" -I../HAL/include -I../LIB -I../Profile/include -I../APP/include -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

