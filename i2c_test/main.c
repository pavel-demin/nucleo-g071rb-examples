#include "stm32g0xx.h"

#define I2C_ADDRESS 0x55

void output(uint32_t buffer)
{
  if(buffer == 0x12345678) GPIOA->ODR |= GPIO_ODR_OD5_Msk;
  else if(buffer == 0x87654321) GPIOA->ODR &= ~GPIO_ODR_OD5_Msk;
}

int main()
{
  uint32_t buffer = 0;

  // enable GPIOA and GPIOB
  RCC->IOPENR |= RCC_IOPENR_GPIOBEN_Msk | RCC_IOPENR_GPIOAEN_Msk;

  // configure PA5 as output
  GPIOA->MODER &= ~GPIO_MODER_MODE5_1;

  // configure PB8 as SCL
  GPIOB->MODER &= ~GPIO_MODER_MODE8_0;
  GPIOB->OTYPER |= GPIO_OTYPER_OT8_Msk;
  GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED8_1;
  GPIOB->PUPDR |= GPIO_PUPDR_PUPD8_0;
  GPIOB->AFR[1] |= GPIO_AFRH_AFSEL8_2 | GPIO_AFRH_AFSEL8_1;

  // configure PB9 as SDA
  GPIOB->MODER &= ~GPIO_MODER_MODE9_0;
  GPIOB->OTYPER |= GPIO_OTYPER_OT9_Msk;
  GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED9_1;
  GPIOB->PUPDR |= GPIO_PUPDR_PUPD9_0;
  GPIOB->AFR[1] |= GPIO_AFRH_AFSEL9_2 | GPIO_AFRH_AFSEL9_1;

  // enable clock for DMA1
  RCC->AHBENR |= RCC_AHBENR_DMA1EN_Msk;

  // configure DMA
  DMAMUX1_Channel0->CCR |= DMAMUX_CxCR_DMAREQ_ID_3 | DMAMUX_CxCR_DMAREQ_ID_1;
  DMA1_Channel1->CCR |= DMA_CCR_MINC_Msk;
  DMA1_Channel1->CPAR = (uint32_t)&I2C1->RXDR;
  DMA1_Channel1->CMAR = (uint32_t)&buffer;

  // set I2C1 clock source to HSI16
  RCC->CCIPR |= RCC_CCIPR_I2C1SEL_1;

  // enable clock for I2C1 and PWR
  RCC->APBENR1 |= RCC_APBENR1_PWREN_Msk | RCC_APBENR1_I2C1EN_Msk;

  // enable I2C1 interrupts, DMA and wakeup from Stop 1 mode
  I2C1->CR1 |= I2C_CR1_WUPEN_Msk | I2C_CR1_RXDMAEN_Msk | I2C_CR1_STOPIE_Msk | I2C_CR1_ADDRIE_Msk;

  // enable I2C1
  I2C1->CR1 |= I2C_CR1_PE_Msk;
  // set I2C1 address 1
  I2C1->OAR1 |= I2C_ADDRESS << 1;
  // enable I2C1 address 1
  I2C1->OAR1 |= I2C_OAR1_OA1EN_Msk;

  // enable Stop 1 mode
  PWR->CR1 |= PWR_CR1_LPMS_0;
  SCB->SCR |= SCB_SCR_SEVONPEND_Msk | SCB_SCR_SLEEPDEEP_Msk;

  while(1)
  {
    __WFE();
    if(I2C1->ISR & I2C_ISR_ADDR_Msk)
    {
      I2C1->ICR |= I2C_ICR_ADDRCF_Msk;
      buffer = 0;
      // enable DMA transfers
      DMA1_Channel1->CCR &= ~DMA_CCR_EN_Msk;
      DMA1_Channel1->CNDTR = 4;
      DMA1_Channel1->CCR |= DMA_CCR_EN_Msk;
      // disable Stop 1 mode
      SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
      // clear interrupt pending bit
      NVIC_ClearPendingIRQ(I2C1_IRQn);
    }
    else if(I2C1->ISR & I2C_ISR_STOPF_Msk)
    {
      I2C1->ICR |= I2C_ICR_STOPCF_Msk;
      output(buffer);
      // enable Stop 1 mode
      SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
      // clear interrupt pending bit
      NVIC_ClearPendingIRQ(I2C1_IRQn);
    }
  }
}
