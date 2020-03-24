#include "print.h"

UartPrint uart1Print(USART2);

Print::Print()
{

}

void Print::begin(uint32_t baud)
{

}

void Print::print(const char *ch)
{
  while(*ch != 0) {
    printOne(*ch);
    ch ++;
  }
}

void Print::print(uint32_t num, uint16_t radix)
{
  int32_t temp1 = 1, temp2;
  while(temp1 <= (num / radix))
    temp1 *= radix;
  while(temp1 >= radix) {
    temp2 = (num / temp1) % radix;
    if(temp2 >= 10)
      printOne('a' + temp2 - 10);
    else
      printOne('0' + temp2);
    temp1 /= radix;
  }
  temp2 = num % radix;
  if(temp2 >= 10)
    printOne('a' + temp2 - 10);
  else
    printOne('0' + temp2);
}

void Print::print(int32_t num, uint16_t radix)
{
  if(num < 0) {
    printOne('-');
    num = -num;
  }
  print((uint32_t) num, radix);
}

void Print::print(double num, uint16_t acc)
{
  if(num < 0) {
    printOne('-');
    num = -num;
  }
  print((int32_t)num);
  printOne('.');
  num = num - (int32_t)num;
  while(acc > 0) {
    num *= 10;
    acc --;
  }
  print((int32_t)num);
}

void Print::println(const char *ch)
{
  print(ch);
  println();
}

void Print::println(int32_t num, uint16_t radix)
{
  print(num, radix);
  println();
}

void Print::println(uint32_t num, uint16_t radix)
{
  print(num, radix);
  println();
}

void Print::println(double num, uint16_t acc)
{
  print(num, acc);
  println();
}

void Print::println()
{
  print("\r\n");
}

UartPrint::UartPrint(USART_TypeDef *uartx)
{
  _uartx = uartx;
}

void UartPrint::begin(uint32_t baud)
{
  LL_USART_InitTypeDef USART_InitStruct = {0};
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_TypeDef *gpiox;
  uint32_t pins;

  switch((uint32_t) _uartx) {
    case ((uint32_t) USART1): gpiox = GPIOA; pins = LL_GPIO_PIN_9 | LL_GPIO_PIN_10;
                              LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
                              LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    break;
    case ((uint32_t) USART2): gpiox = GPIOA; pins = LL_GPIO_PIN_2 | LL_GPIO_PIN_3;
                              LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
                              LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    break;
  }
  GPIO_InitStruct.Pin = pins;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
  LL_GPIO_Init(gpiox, &GPIO_InitStruct);

  USART_InitStruct.BaudRate = baud;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Disable(_uartx);
  LL_USART_Init(_uartx, &USART_InitStruct);
  LL_USART_ConfigAsyncMode(_uartx);
  LL_USART_Enable(_uartx);
}

void UartPrint::printOne(uint8_t ch)
{
  while(! (_uartx->SR & (1 << 7)));
  _uartx->DR = ch;
}

void UartPrint::printBuf(uint8_t *buf, uint16_t len)
{
  while(len --) {
    while(! (_uartx->SR & (1 << 7)));
    _uartx->DR = *buf;
    buf ++;
  }
}
