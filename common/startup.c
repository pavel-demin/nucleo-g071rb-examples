#include "stm32g0xx.h"

extern uint32_t _stack_end;
extern uint32_t _flash_start;
extern uint32_t _text_start;
extern uint32_t _text_end;
extern uint32_t _bss_start;
extern uint32_t _bss_end;

extern int main();

__attribute__((section(".reset_handler"))) void Reset_Handler()
{
  uint32_t *dst, *src;

  // copy .text section to RAM
  dst = &_text_start;
  src = &_flash_start;
  while(dst < &_text_end) *dst++ = *src++;

  // fill .bss section with zeros
  dst = &_bss_start;
  while(dst < &_bss_end) *dst++ = 0;

  // set vector table location
  SCB->VTOR = (uint32_t)&_text_start;

  // call main
  main();
}

void Default_Handler()
{
  while(1);
}

#define DECLARE_HANDLER(name) \
  extern void name() __attribute__((weak, alias("Default_Handler")));

DECLARE_HANDLER(NMI_Handler)
DECLARE_HANDLER(HardFault_Handler)
DECLARE_HANDLER(SVC_Handler)
DECLARE_HANDLER(PendSV_Handler)
DECLARE_HANDLER(SysTick_Handler)
DECLARE_HANDLER(WWDG_IRQHandler)
DECLARE_HANDLER(PVD_IRQHandler)
DECLARE_HANDLER(RTC_TAMP_IRQHandler)
DECLARE_HANDLER(FLASH_IRQHandler)
DECLARE_HANDLER(RCC_IRQHandler)
DECLARE_HANDLER(EXTI0_1_IRQHandler)
DECLARE_HANDLER(EXTI2_3_IRQHandler)
DECLARE_HANDLER(EXTI4_15_IRQHandler)
DECLARE_HANDLER(UCPD1_2_IRQHandler)
DECLARE_HANDLER(DMA1_Channel1_IRQHandler)
DECLARE_HANDLER(DMA1_Channel2_3_IRQHandler)
DECLARE_HANDLER(DMA1_Ch4_7_DMAMUX1_OVR_IRQHandler)
DECLARE_HANDLER(ADC1_COMP_IRQHandler)
DECLARE_HANDLER(TIM1_BRK_UP_TRG_COM_IRQHandler)
DECLARE_HANDLER(TIM1_CC_IRQHandler)
DECLARE_HANDLER(TIM2_IRQHandler)
DECLARE_HANDLER(TIM3_IRQHandler)
DECLARE_HANDLER(TIM6_DAC_LPTIM1_IRQHandler)
DECLARE_HANDLER(TIM7_LPTIM2_IRQHandler)
DECLARE_HANDLER(TIM14_IRQHandler)
DECLARE_HANDLER(TIM15_IRQHandler)
DECLARE_HANDLER(TIM16_IRQHandler)
DECLARE_HANDLER(TIM17_IRQHandler)
DECLARE_HANDLER(I2C1_IRQHandler)
DECLARE_HANDLER(I2C2_IRQHandler)
DECLARE_HANDLER(SPI1_IRQHandler)
DECLARE_HANDLER(SPI2_IRQHandler)
DECLARE_HANDLER(USART1_IRQHandler)
DECLARE_HANDLER(USART2_IRQHandler)
DECLARE_HANDLER(USART3_4_LPUART1_IRQHandler)
DECLARE_HANDLER(CEC_IRQHandler)

const void *vector_table[] __attribute__((section(".vector_table"), used)) =
{
  &_stack_end,
  &Reset_Handler,
  &NMI_Handler,
  &HardFault_Handler,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  &SVC_Handler,
  0,
  0,
  &PendSV_Handler,
  &SysTick_Handler,
  &WWDG_IRQHandler,
  &PVD_IRQHandler,
  &RTC_TAMP_IRQHandler,
  &FLASH_IRQHandler,
  &RCC_IRQHandler,
  &EXTI0_1_IRQHandler,
  &EXTI2_3_IRQHandler,
  &EXTI4_15_IRQHandler,
  &UCPD1_2_IRQHandler,
  &DMA1_Channel1_IRQHandler,
  &DMA1_Channel2_3_IRQHandler,
  &DMA1_Ch4_7_DMAMUX1_OVR_IRQHandler,
  &ADC1_COMP_IRQHandler,
  &TIM1_BRK_UP_TRG_COM_IRQHandler,
  &TIM1_CC_IRQHandler,
  &TIM2_IRQHandler,
  &TIM3_IRQHandler,
  &TIM6_DAC_LPTIM1_IRQHandler,
  &TIM7_LPTIM2_IRQHandler,
  &TIM14_IRQHandler,
  &TIM15_IRQHandler,
  &TIM16_IRQHandler,
  &TIM17_IRQHandler,
  &I2C1_IRQHandler,
  &I2C2_IRQHandler,
  &SPI1_IRQHandler,
  &SPI2_IRQHandler,
  &USART1_IRQHandler,
  &USART2_IRQHandler,
  &USART3_4_LPUART1_IRQHandler,
  &CEC_IRQHandler
};
