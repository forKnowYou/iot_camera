#ifndef ILI9341_H
#define ILI9341_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_sram.h"
#include "stm32f4xx_hal_gpio.h"

#include "pin.h"
#include "print.h"
#include "timer.h"

class ILI9341 {
public:
  typedef enum {
    rotation0,
    rotation90,
    rotation180,
    rotation270
  } rotation_t;

  ILI9341(Pin bkLedPin, uint16_t nex, Pin cdPin, uint16_t cdOffset, uint16_t width, uint16_t height);

  void begin(void);
  void gpioInit(void);

  void setCursor(uint16_t xs, uint16_t xe, uint16_t ys, uint16_t ye);
  void setWriteToGram();
  void setScanRotation(rotation_t rot);

  void fillRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color);
  void bkLedON(void);
  void bkLedOFF(void);
  void setRotation(rotation_t rot);

  void writeCmd(uint16_t cmd);
  void writeDat(uint16_t dat);
  void writeDatBuffer(uint16_t *buf, uint32_t len);
  uint16_t readDat(void);

  SRAM_HandleTypeDef _hsram;

private:
  Pin _bkLedPin, _cdPin;
  const uint16_t _nex;
  const uint16_t _cdOffset;
  const uint16_t _rowWidth, _rowHeight;
  uint16_t _width, _height;
  uint16_t _id;
  uint32_t _datAddr, _cmdAddr;
  rotation_t _rotation;
};

extern ILI9341 ili;

#endif
