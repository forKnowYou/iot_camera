#ifndef SOFTIIC_H
#define SOFTIIC_H

#include "stdint.h"

#include "stm32f4xx_hal.h"

#include "pin.h"

class SoftIIC {
public:
  typedef void (*funDelay)(void);

  SoftIIC(Pin scl, Pin sda, funDelay fDelay);

  void begin();
  bool write(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len);
  bool read(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len);

private:
  void start();
  void end();
  void writeByte(uint8_t dat);
  uint8_t readByte();
  void sendRes(bool res);
  bool checkRes();

  Pin _scl, _sda;
  const funDelay _funDelay;
};

#endif
