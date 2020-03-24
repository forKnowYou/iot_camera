#include "timer.h"

BaseTimer delayTimer(TIM6);

BaseTimer::BaseTimer(TIM_TypeDef *timer) : _freq(84000000)
{
  _timer = timer;
}

void BaseTimer::begin()
{
  __HAL_RCC_TIM6_CLK_ENABLE();
  __HAL_RCC_TIM7_CLK_ENABLE();

  _timer->PSC = _freq / 1000000 - 1;
  _timer->DIER = 0x00;
}

void BaseTimer::delayMs(uint16_t t)
{
  while(t--)
    delayUs(1000);
}

void BaseTimer::delayUs(uint16_t t)
{
  _timer->CNT = 0xffff - t;
  _timer->CR1 = 0x09;
  while(! (_timer->SR & 0x01));
  _timer->SR &= 0xfffffffe;
  _timer->CR1 = 0x00;
}
