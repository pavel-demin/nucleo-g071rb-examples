#include "stm32g0xx.h"

int main()
{
  // enable GPIOA
  RCC->IOPENR |= RCC_IOPENR_GPIOAEN_Msk;

  // configure PA5 as output
  GPIOA->MODER &= ~GPIO_MODER_MODE5_1;

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
  LPTIM1->ARR = 16000;

  // start LPTIM1 counter
  LPTIM1->CR |= LPTIM_CR_CNTSTRT_Msk;

  while(1)
  {
    __WFE();
    if(LPTIM1->ISR & LPTIM_ISR_ARRM_Msk)
    {
      LPTIM1->ICR |= LPTIM_ICR_ARRMCF_Msk;
      GPIOA->ODR ^= GPIO_ODR_OD5_Msk;
      // clear interrupt pending bit
      NVIC_ClearPendingIRQ(TIM6_DAC_LPTIM1_IRQn);
    }
  }
}
