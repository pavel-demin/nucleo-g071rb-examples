#include "stm32g0xx.h"

#define I2C_ADDRESS 0x55

uint32_t i2c_buffer[2] = {0};
uint16_t spi_buffer[5] = {0};

uint8_t ready = 0;

void output()
{
  int32_t i;
  uint32_t tx, lpf, ptt;
  uint32_t code[3], data[3];
  uint16_t bits[6];
  uint8_t att[2];

  tx = i2c_buffer[0] & 0x1;
  ptt = (i2c_buffer[0] >> 1) & 0x1;
  att[0] = i2c_buffer[0] >> 20 & 0x1F;
  att[1] = i2c_buffer[0] >> 25 & 0x1F;

  code[2] = i2c_buffer[1] >> 4 & 0xF;
  code[1] = i2c_buffer[1] & 0xF;
  code[0] = tx ? code[2] : code[1];

  lpf = 0;
  if(code[0] == 2) lpf = 1;
  else if(code[0] == 3 || code[0] == 4) lpf = 2;
  else if(code[0] == 5 || code[0] == 6) lpf = 3;
  else if(code[0] == 7 || code[0] == 8) lpf = 4;
  else if(code[0] == 9 || code[0] == 10) lpf = 5;
  else if(code[0] == 11 || code[0] == 0) lpf = 6;

  data[0] = code[1] << 28 | i2c_buffer[0] >> 2;

  data[1] = code[2];
  data[1] |= 1 << (code[0] + 4);
  data[1] |= 1 << (code[1] + 16);
  data[1] |= 1 << (code[2] + 28);

  data[2] = (code[2] > 3) ? 1 << (code[2] - 4) : 0;
  data[2] |= 1 << (lpf + 8);
  data[2] |= tx << 15;
  data[2] |= (ptt && tx == 0) << 16;
  data[2] |= (ptt && tx == 1) << 17;
  data[2] |= ptt << 18;
  data[2] |= (att[0] == 0) << 19;
  data[2] |= (att[1] == 0) << 20;

  bits[0] = ((data[1] >> 16) & 0x0fff) | ((data[2] << 4) & 0xf000);
  bits[1] = ((data[0] << 3) & 0x03f8) | ((data[0] >> 2) & 0x1c00) | ((data[1] >> 15) & 0xe000) | ((data[2] >> 12) & 0x0007);
  bits[2] = ((data[0] << 5) & 0x3000) | ((data[0] >> 0) & 0x0e00) | ((data[0] >> 1) & 0xc000) | ((data[1] >> 31) & 0x0001) | ((data[2] << 1) & 0x01fe);
  bits[3] = ((data[0] >> 15) & 0x0004) | ((data[0] >> 24) & 0x00f0) | ((data[1] << 8) & 0xff00) | ((data[2] >> 12) & 0x0008) | ((data[2] >> 16) & 0x0003);
  bits[4] = ((data[1] >> 8) & 0x00ff) | ((data[2] >> 10) & 0x0700);
  bits[5] = ((data[0] >> 5) & 0x4000) | ((data[0] >> 10) & 0x8000) | ((data[0] >> 14) & 0x2000) | ((data[0] >> 16) & 0x0030) | ((data[0] >> 16) & 0x0400) | ((data[0] >> 18) & 0x0001) | ((data[0] >> 18) & 0x0040) | ((data[0] >> 19) & 0x0008) | ((data[0] >> 21) & 0x0004);

  for(i = 0; i < 5; ++i)
  {
    spi_buffer[i] = bits[4 - i];
  }

  // enable DMA transfers
  DMA1_Channel2->CCR &= ~DMA_CCR_EN_Msk;
  DMA1_Channel2->CNDTR = 5;
  DMA1_Channel2->CCR |= DMA_CCR_EN_Msk;

  // wait for the end of the transfer
  while(SPI1->SR & SPI_SR_FTLVL_Msk);
  while(SPI1->SR & SPI_SR_BSY_Msk);

  // generate strobe pulse
  GPIOA->ODR |= GPIO_ODR_OD6_Msk;
  GPIOA->ODR &= ~GPIO_ODR_OD6_Msk;

  // output ATT1 and ATT2 signals
  GPIOB->ODR = (GPIOB->ODR & 0xFFFF1B82) | bits[5];
}

int main()
{
  // enable GPIOA and GPIOB
  RCC->IOPENR |= RCC_IOPENR_GPIOBEN_Msk | RCC_IOPENR_GPIOAEN_Msk;

  // configure PB as output
  GPIOB->MODER &= 0x57DFD55D;

  // configure PA6 as output
  GPIOA->MODER &= ~GPIO_MODER_MODE6_1;
  GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED6_1;

  // configure PA5 as SCK
  GPIOA->MODER &= ~GPIO_MODER_MODE5_0;
  GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED5_1;

  // configure PA7 as MOSI
  GPIOA->MODER &= ~GPIO_MODER_MODE7_0;
  GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED7_1;

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

  // configure DMA for I2C
  DMAMUX1_Channel0->CCR |= DMAMUX_CxCR_DMAREQ_ID_3 | DMAMUX_CxCR_DMAREQ_ID_1;
  DMA1_Channel1->CCR |= DMA_CCR_MINC_Msk;
  DMA1_Channel1->CPAR = (uint32_t)&I2C1->RXDR;
  DMA1_Channel1->CMAR = (uint32_t)i2c_buffer;

  // configure DMA for SPI
  DMAMUX1_Channel1->CCR |= DMAMUX_CxCR_DMAREQ_ID_4 | DMAMUX_CxCR_DMAREQ_ID_0;
  DMA1_Channel2->CCR |= DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 | DMA_CCR_MINC_Msk | DMA_CCR_DIR_Msk;
  DMA1_Channel2->CPAR = (uint32_t)&SPI1->DR;
  DMA1_Channel2->CMAR = (uint32_t)spi_buffer;

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

  // enable clock for SPI1
  RCC->APBENR2 |= RCC_APBENR2_SPI1EN_Msk;

  // configure SPI1
  SPI1->CR1 |= SPI_CR1_SSM_Msk | SPI_CR1_SSI_Msk | SPI_CR1_BR_0 | SPI_CR1_MSTR_Msk;
  SPI1->CR2 |= SPI_CR2_DS_3 | SPI_CR2_TXDMAEN_Msk;
  // enable SPI1
  SPI1->CR1 |= SPI_CR1_SPE_Msk;

  i2c_buffer[0] = 0x02100000;
  output();

  // enable Stop 1 mode
  PWR->CR1 |= PWR_CR1_LPMS_0;
  SCB->SCR |= SCB_SCR_SEVONPEND_Msk | SCB_SCR_SLEEPDEEP_Msk;

  while(1)
  {
    __WFE();
    if(I2C1->ISR & I2C_ISR_ADDR_Msk)
    {
      I2C1->ICR |= I2C_ICR_ADDRCF_Msk;
      i2c_buffer[0] = 0;
      i2c_buffer[1] = 0;
      // enable DMA transfers
      DMA1_Channel1->CCR &= ~DMA_CCR_EN_Msk;
      DMA1_Channel1->CNDTR = 5;
      DMA1_Channel1->CCR |= DMA_CCR_EN_Msk;
      // disable Stop 1 mode
      SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
      // clear interrupt pending bit
      NVIC_ClearPendingIRQ(I2C1_IRQn);
    }
    else if(I2C1->ISR & I2C_ISR_STOPF_Msk)
    {
      I2C1->ICR |= I2C_ICR_STOPCF_Msk;
      ready = 1;
      // enable Stop 1 mode
      SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
      // clear interrupt pending bit
      NVIC_ClearPendingIRQ(I2C1_IRQn);
    }
    if(ready)
    {
      ready = 0;
      output();
    }
  }
}
