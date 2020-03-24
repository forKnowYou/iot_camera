#ifndef LED_H
#define LED_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

#include "pin.h"

class LED {
public:
  LED(Pin pin);

  void begin(void);

  void ON(void);
  void OFF(void);

private:
  Pin _pin;
};

extern LED led0;
extern LED led1;

#endif
