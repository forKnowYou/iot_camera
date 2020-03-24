#ifndef TIMER_H
#define TIMER_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"

class BaseTimer {
public:
  BaseTimer(TIM_TypeDef *timer);

  void begin();
  void delayMs(uint16_t t);
  void delayUs(uint16_t t);

private:
  TIM_TypeDef *_timer;

  uint32_t const _freq;
};

extern BaseTimer delayTimer;

#endif
