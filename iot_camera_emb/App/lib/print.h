#ifndef PRINT_H
#define PRINT_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"

class Print {
public:
  Print();

  virtual void begin(uint32_t baud) = 0;

  virtual void print(const char *ch);
  virtual void print(int32_t num, uint16_t radix = 10);
  virtual void print(uint32_t num, uint16_t radix = 10);
  virtual void print(double num, uint16_t acc = 2);
  virtual void println(const char *ch);
  virtual void println(int32_t num, uint16_t radix = 10);
  virtual void println(uint32_t num, uint16_t radix = 10);
  virtual void println(double num, uint16_t acc = 2);
  virtual void println(void);
  virtual void printOne(uint8_t ch) = 0;
};

class UartPrint : public Print {
public:
  UartPrint(USART_TypeDef *uartx);

  virtual void begin(uint32_t baud);
  virtual void printOne(uint8_t ch);
  void printBuf(uint8_t *buf, uint16_t len);

  USART_TypeDef *_uartx;
};

extern UartPrint uart1Print;

#endif
