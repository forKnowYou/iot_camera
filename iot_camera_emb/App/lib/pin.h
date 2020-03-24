#ifndef PIN_H
#define PIN_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_ll_gpio.h"

class Pin {
public:
  Pin(GPIO_TypeDef *gpiox, uint16_t pin);

  enum Speed {
    low,
    medium,
    fast,
    high
  };

  enum OutType {
    pp,
    od
  };

  enum Pull {
    pullUp,
    pullDown
  };

  void setOut(Speed speed, OutType outType);
  void setInput(Pull pull);

  void set();
  void reset();
  uint16_t read();

  GPIO_TypeDef *_gpiox;
  uint16_t _pin;
};

#endif
