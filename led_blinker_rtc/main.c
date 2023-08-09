#include "stm32g0xx.h"

int main()
{
  // enable GPIOA
  RCC->IOPENR |= RCC_IOPENR_GPIOAEN_Msk;

  // configure PA5 as output
  GPIOA->MODER &= ~GPIO_MODER_MODE5_1;

  // enable LSI
  RCC->CSR |= RCC_CSR_LSION_Msk;

  // enable clock for RTC and PWR
  RCC->APBENR1 |= RCC_APBENR1_RTCAPBEN_Msk | RCC_APBENR1_PWREN_Msk;

  // enable access to RTC registers
  PWR->CR1 |= PWR_CR1_DBP_Msk;

  // enable RTC and set RTC clock source to LSI
  RCC->BDCR |= RCC_BDCR_RTCEN_Msk | RCC_BDCR_RTCSEL_1;

  // disable RTC write protection
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  // disable RTC wakeup timer
  RTC->CR &= ~RTC_CR_WUTE_Msk;

  // wait for RTC wakeup timer write flag
  while(!(RTC->ICSR & RTC_ICSR_WUTWF_Msk));

  // set RTC wakeup clock to RTC/2
  RTC->CR |= RTC_CR_WUCKSEL_1 | RTC_CR_WUCKSEL_0;

  // set RTC wakeup autoreload register
  RTC->WUTR = 8000;

  // enable RTC wakeup timer and interrup
  RTC->CR |= RTC_CR_WUTIE_Msk | RTC_CR_WUTE_Msk;

  // enable RTC write protection
  RTC->WPR = 0xFF;

  // enable RTC events
  EXTI->EMR1 |= EXTI_EMR1_EM19_Msk;

  // enable Stop 1 mode
  PWR->CR1 |= PWR_CR1_LPMS_0;
  SCB->SCR |= SCB_SCR_SEVONPEND_Msk | SCB_SCR_SLEEPDEEP_Msk;

  // clear RTC wakeup timer flag
  RTC->SCR |= RTC_SCR_CWUTF_Msk;

  while(1)
  {
    __WFE();
    if(RTC->SR & RTC_SR_WUTF_Msk)
    {
      RTC->SCR |= RTC_SCR_CWUTF_Msk;
      GPIOA->ODR ^= GPIO_ODR_OD5_Msk;
      // clear interrupt pending bit
      NVIC_ClearPendingIRQ(RTC_TAMP_IRQn);
    }
  }
}
