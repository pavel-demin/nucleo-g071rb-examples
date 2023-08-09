#include "stm32g0xx.h"

int main()
{
  int32_t i;

  // enable GPIOA
  RCC->IOPENR |= RCC_IOPENR_GPIOAEN_Msk;

  // configure PA5 as output
  GPIOA->MODER &= ~GPIO_MODER_MODE5_1;

  while(1)
  {
    // delay
    for(i = 0; i < 2000000; ++i) __NOP();

    // toggle PA5
    GPIOA->ODR ^= GPIO_ODR_OD5_Msk;
  }
}
