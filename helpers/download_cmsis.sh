#! /bin/sh

STM32G0_URL=https://raw.githubusercontent.com/STMicroelectronics/STM32CubeG0/master

mkdir CMSIS

HEADERS="\
  Include/cmsis_compiler.h \
  Include/cmsis_gcc.h \
  Include/cmsis_version.h \
  Include/core_cm0plus.h \
  Include/mpu_armv7.h \
  Device/ST/STM32G0xx/Include/stm32g071xx.h \
  Device/ST/STM32G0xx/Include/stm32g0xx.h \
  Device/ST/STM32G0xx/Include/system_stm32g0xx.h \
"

for file in $HEADERS
do
  name=`basename $file`
  curl -L $STM32G0_URL/Drivers/CMSIS/$file -o CMSIS/$name
  sed -i 's/\ *$//' CMSIS/$name
done
