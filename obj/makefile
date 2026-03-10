################################################################################
# MRS Version: 1.9.2
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include StdPeriphDriver/subdir.mk
-include Startup/subdir.mk
-include RVMSIS/subdir.mk
-include Profile/subdir.mk
-include HAL/subdir.mk
-include APP/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
A2200BSH_CH582M_OTA_APP_HP203B.hexAndbin \

SECONDARY_LIST += \
A2200BSH_CH582M_OTA_APP_HP203B.lst \

SECONDARY_SIZE += \
A2200BSH_CH582M_OTA_APP_HP203B.siz \


# All Target
all: A2200BSH_CH582M_OTA_APP_HP203B.elf secondary-outputs

# Tool invocations
A2200BSH_CH582M_OTA_APP_HP203B.elf: $(OBJS) $(USER_OBJS)
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common  -g -T "../Ld/Link.ld" -nostartfiles -Xlinker --gc-sections -L"../" -L"../StdPeriphDriver" -L../LIB -Xlinker --print-memory-usage -Wl,-Map,"A2200BSH_CH582M_OTA_APP_HP203B.map" --specs=nano.specs --specs=nosys.specs -o "A2200BSH_CH582M_OTA_APP_HP203B.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@	@
A2200BSH_CH582M_OTA_APP_HP203B.hexAndbin: A2200BSH_CH582M_OTA_APP_HP203B.elf
	@	riscv-none-embed-objcopy -O ihex "A2200BSH_CH582M_OTA_APP_HP203B.elf"  "A2200BSH_CH582M_OTA_APP_HP203B.hex" && riscv-none-embed-objcopy -O binary "A2200BSH_CH582M_OTA_APP_HP203B.elf"  "A2200BSH_CH582M_OTA_APP_HP203B.bin"
	@	@
A2200BSH_CH582M_OTA_APP_HP203B.lst: A2200BSH_CH582M_OTA_APP_HP203B.elf
	@	riscv-none-embed-objdump --source --all-headers --demangle --line-numbers --wide "A2200BSH_CH582M_OTA_APP_HP203B.elf" > "A2200BSH_CH582M_OTA_APP_HP203B.lst"
	@	@
A2200BSH_CH582M_OTA_APP_HP203B.siz: A2200BSH_CH582M_OTA_APP_HP203B.elf
	@	riscv-none-embed-size --format=berkeley "A2200BSH_CH582M_OTA_APP_HP203B.elf"
	@	@
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(ASM_UPPER_DEPS)$(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS) A2200BSH_CH582M_OTA_APP_HP203B.elf
	-@
post-build:
	-riscv-none-embed-objcopy -O ihex "A2200BSH_CH582M_OTA_APP_HP203B.elf" "A2200BSH_CH582M_OTA_APP_HP203B.hex" && riscv-none-embed-objcopy -O binary "A2200BSH_CH582M_OTA_APP_HP203B.elf" "A2200BSH_CH582M_OTA_APP_HP203B.bin"
	-@
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents post-build

-include ../makefile.targets
