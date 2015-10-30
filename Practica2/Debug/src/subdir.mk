################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/MyFileSystem.c \
../src/fuseLib.c \
../src/myFS.c 

OBJS += \
./src/MyFileSystem.o \
./src/fuseLib.o \
./src/myFS.o 

C_DEPS += \
./src/MyFileSystem.d \
./src/fuseLib.d \
./src/myFS.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -D_FILE_OFFSET_BITS=64 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


