#include "pin.h"

Pin::Pin(GPIO_TypeDef *gpiox, uint16_t pin)
{
  _gpiox = gpiox;
  _pin = pin;
}

void Pin::setOut(Speed speed, OutType outType)
{
  GPIO_InitTypeDef init;
  switch(speed) {
    case low: init.Speed = GPIO_SPEED_FREQ_LOW; break;
    case medium: init.Speed = GPIO_SPEED_FREQ_MEDIUM; break;
    case fast: init.Speed = GPIO_SPEED_FREQ_HIGH; break;
    case high: init.Speed = GPIO_SPEED_FREQ_VERY_HIGH; break;
  }
  switch(outType) {
    case pp: init.Mode = GPIO_MODE_OUTPUT_PP; break;
    case od: init.Mode = GPIO_MODE_OUTPUT_OD; init.Pull = GPIO_PULLUP; break;
  }
  init.Pin = _pin;
  HAL_GPIO_Init(_gpiox, &init);
}

void Pin::setInput(Pull pull)
{
  GPIO_InitTypeDef init;
  switch(pull) {
    case pullUp: init.Pull = GPIO_PULLUP; break;
    case pullDown: init.Pull = GPIO_PULLDOWN; break;
  }
  init.Pin = _pin;
  init.Mode = GPIO_MODE_INPUT;
  HAL_GPIO_Init(_gpiox, &init);
}

void Pin::set()
{
  // HAL_GPIO_WritePin(_gpiox, _pin, GPIO_PIN_SET);
  _gpiox->ODR |= _pin;
}

void Pin::reset()
{
  // HAL_GPIO_WritePin(_gpiox, _pin, GPIO_PIN_RESET);
  _gpiox->ODR &= ~_pin;
}

uint16_t Pin::read()
{
  return HAL_GPIO_ReadPin(_gpiox, _pin);
}
