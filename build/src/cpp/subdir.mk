# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/cpp/backbone_coor.cpp \
../src/cpp/bktree_scanner.cpp \
../src/cpp/cliq_value.cpp \
../src/cpp/clique_key.cpp \
../src/cpp/dllib.cpp \
../src/cpp/dp_builder.cpp \
../src/cpp/pattern.cpp 

OBJS += \
./src/cpp/backbone_coor.o \
./src/cpp/bktree_scanner.o \
./src/cpp/cliq_value.o \
./src/cpp/clique_key.o \
./src/cpp/dllib.o \
./src/cpp/dp_builder.o \
./src/cpp/pattern.o 

CPP_DEPS += \
./src/cpp/backbone_coor.d \
./src/cpp/bktree_scanner.d \
./src/cpp/cliq_value.d \
./src/cpp/clique_key.d \
./src/cpp/dllib.d \
./src/cpp/dp_builder.d \
./src/cpp/pattern.d 


# Each subdirectory must supply rules for building sources it contributes
src/cpp/%.o: ../../src/cpp/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"../src/include" -O2 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


