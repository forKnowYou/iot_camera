#include "led.h"

LED led0(Pin(GPIOF, GPIO_PIN_9));
LED led1(Pin(GPIOF, GPIO_PIN_10));

LED::LED(Pin pin): _pin(pin)
{

}

void LED::begin()
{
  _pin.setOut(Pin::low, Pin::pp);
  ON();
}

void LED::ON(void)
{
  _pin.reset();
}

void LED::OFF(void)
{
  _pin.set();
}

