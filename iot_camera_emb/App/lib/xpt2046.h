#ifndef XPT2046_H
#define XPT2046_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_spi.h"

#include "pin.h"

class XPT2046 {
public:
  XPT2046(SPI_TypeDef *spix, Pin csPin, uint32_t screenXMax, uint32_t screenYMax);

  void begin();

  struct {
    uint32_t x;
    uint32_t y;
  } _point;

  void calibrate();
  void read();

  const uint32_t _xMax, _yMax;
  const uint32_t _screenXMax, _screenYMax;

private:
  void writeCmd(uint8_t cmd);
  void readDat(uint8_t *buf, uint16_t len);

  Pin _csPin;
  SPI_HandleTypeDef _hspi;
};

extern XPT2046 xpt;

#endif
