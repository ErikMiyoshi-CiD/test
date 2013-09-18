################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS :=  \
../cmsis/ \
../cmsis/linkerScripts/ \
../cmsis/src/


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../cmsis/src/startup_samd20.c \
../cmsis/src/system_samd20.c \
../leitorrf.c


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
cmsis/src/startup_samd20.o \
cmsis/src/system_samd20.o \
leitorrf.o

OBJS_AS_ARGS +=  \
cmsis/src/startup_samd20.o \
cmsis/src/system_samd20.o \
leitorrf.o

C_DEPS +=  \
cmsis/src/startup_samd20.d \
cmsis/src/system_samd20.d \
leitorrf.d

C_DEPS_AS_ARGS +=  \
cmsis/src/startup_samd20.d \
cmsis/src/system_samd20.d \
leitorrf.d

OUTPUT_FILE_PATH +=leitorrf.elf

OUTPUT_FILE_PATH_AS_ARGS +=leitorrf.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

# AVR32/GNU C Compiler







cmsis/src/%.o: ../cmsis/src/%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 4.7.3
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\ARM GCC\Native\4.7.3.1029\arm-gnu-toolchain\bin\arm-none-eabi-gcc.exe$(QUOTE)  -mthumb -D__SAMD20E16__ -DDEBUG  -I"$(ToolchainDir)\..\..\CMSIS_Atmel" -I"$(ToolchainDir)\..\..\CMSIS_Atmel\CMSIS\Include" -I"$(ToolchainDir)\..\..\CMSIS_Atmel\Device\ATMEL" -I"$(ToolchainDir)\..\..\CMSIS_Atmel\Device\ATMEL\samd20\include"  -O1 -ffunction-sections -mlong-calls -g3 -Wall -mcpu=cortex-m0plus -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<"
	@echo Finished building: $<
	

./%.o: .././%.c
	@echo Building file: $<
	@echo Invoking: ARM/GNU C Compiler : 4.7.3
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\ARM GCC\Native\4.7.3.1029\arm-gnu-toolchain\bin\arm-none-eabi-gcc.exe$(QUOTE)  -mthumb -D__SAMD20E16__ -DDEBUG  -I"$(ToolchainDir)\..\..\CMSIS_Atmel" -I"$(ToolchainDir)\..\..\CMSIS_Atmel\CMSIS\Include" -I"$(ToolchainDir)\..\..\CMSIS_Atmel\Device\ATMEL" -I"$(ToolchainDir)\..\..\CMSIS_Atmel\Device\ATMEL\samd20\include"  -O1 -ffunction-sections -mlong-calls -g3 -Wall -mcpu=cortex-m0plus -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<"
	@echo Finished building: $<
	



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP) $(LIB_DEP)
	@echo Building target: $@
	@echo Invoking: ARM/GNU Linker : 4.7.3
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\ARM GCC\Native\4.7.3.1029\arm-gnu-toolchain\bin\arm-none-eabi-gcc.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -mthumb -Wl,-Map="leitorrf.map" -Wl,--start-group -lm  -Wl,--end-group -L"../cmsis/linkerScripts"  -Wl,--gc-sections -mcpu=cortex-m0plus -Tsamd20e14_flash.ld 
	@echo Finished building target: $@
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\ARM GCC\Native\4.7.3.1029\arm-gnu-toolchain\bin\arm-none-eabi-objcopy.exe" -O binary "leitorrf.elf" "leitorrf.bin"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\ARM GCC\Native\4.7.3.1029\arm-gnu-toolchain\bin\arm-none-eabi-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature  "leitorrf.elf" "leitorrf.hex"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\ARM GCC\Native\4.7.3.1029\arm-gnu-toolchain\bin\arm-none-eabi-objcopy.exe" -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings -O binary "leitorrf.elf" "leitorrf.eep" || exit 0
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\ARM GCC\Native\4.7.3.1029\arm-gnu-toolchain\bin\arm-none-eabi-objdump.exe" -h -S "leitorrf.elf" > "leitorrf.lss"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\ARM GCC\Native\4.7.3.1029\arm-gnu-toolchain\bin\arm-none-eabi-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature  "leitorrf.elf" "leitorrf.srec"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\ARM GCC\Native\4.7.3.1029\arm-gnu-toolchain\bin\arm-none-eabi-size.exe" "leitorrf.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "leitorrf.elf" "leitorrf.a" "leitorrf.hex" "leitorrf.bin" "leitorrf.lss" "leitorrf.eep" "leitorrf.map" "leitorrf.srec"
	