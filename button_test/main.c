#include "stm32g0xx.h"

uint8_t ready = 0;
uint8_t state = 0;
uint8_t button = 0;

void input()
{
  state = (state << 1) | ((GPIOC->IDR & GPIO_IDR_ID13_Msk) == GPIO_IDR_ID13_Msk);
  if(state == 0x80)
  {
    ready = 1;
    button = 1;
  }
  else if(state == 0x7F)
  {
    ready = 1;
    button = 0;
  }
}

void output()
{
  if(button)
  {
    GPIOA->ODR |= GPIO_ODR_OD5_Msk;
  }
  else
  {
    GPIOA->ODR &= ~GPIO_ODR_OD5_Msk;
  }
}

int main()
{
  // enable GPIOA and GPIOC
  RCC->IOPENR |= RCC_IOPENR_GPIOCEN_Msk | RCC_IOPENR_GPIOAEN_Msk;

  // configure PA5 as output
  GPIOA->MODER &= ~GPIO_MODER_MODE5_1;

  // configure PC13 as input
  GPIOC->MODER &= ~GPIO_MODER_MODE13_Msk;

  // enable LSI
  RCC->CSR |= RCC_CSR_LSION_Msk;

  // set LPTIM1 clock source to LSI
  RCC->CCIPR |= RCC_CCIPR_LPTIM1SEL_0;

  // enable clock for LPTIM1 and PWR
  RCC->APBENR1 |= RCC_APBENR1_LPTIM1EN_Msk | RCC_APBENR1_PWREN_Msk;

  // enable ARRM interrupt
  LPTIM1->IER |= LPTIM_IER_ARRMIE_Msk;

  // enable LPTIM1
  LPTIM1->CR |= LPTIM_CR_ENABLE_Msk;

  // enable Stop 1 mode
  PWR->CR1 |= PWR_CR1_LPMS_0;
  SCB->SCR |= SCB_SCR_SEVONPEND_Msk | SCB_SCR_SLEEPDEEP_Msk;

  // set LPTIM1 autoreload register
  LPTIM1->ARR = 160;

  // start LPTIM1 counter
  LPTIM1->CR |= LPTIM_CR_CNTSTRT_Msk;

  while(1)
  {
    __WFE();
    if(LPTIM1->ISR & LPTIM_ISR_ARRM_Msk)
    {
      LPTIM1->ICR |= LPTIM_ICR_ARRMCF_Msk;
      input();
      // clear interrupt pending bit
      NVIC_ClearPendingIRQ(TIM6_DAC_LPTIM1_IRQn);
    }
    if(ready)
    {
      ready = 0;
      output();
    }
  }
}
