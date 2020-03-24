#include "xpt2046.h"

XPT2046 xpt(SPI1, Pin(GPIOC, GPIO_PIN_13), 240, 320);

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(spiHandle->Instance==SPI1)
  {
    /* SPI1 clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }
}

XPT2046::XPT2046(SPI_TypeDef *spix, Pin csPin, uint32_t screenXMax, uint32_t screenYMax): \
  _xMax(4096), _yMax(4096), _screenXMax(screenXMax), _screenYMax(screenYMax), _csPin(csPin)
{
  _hspi.Instance = spix;
}

void XPT2046::begin()
{
  _csPin.setOut(Pin::high, Pin::pp);
  // _hspi.Instance = SPI1;
  _hspi.Init.Mode = SPI_MODE_MASTER;
  _hspi.Init.Direction = SPI_DIRECTION_2LINES;
  _hspi.Init.DataSize = SPI_DATASIZE_8BIT;
  _hspi.Init.CLKPolarity = SPI_POLARITY_HIGH;
  _hspi.Init.CLKPhase = SPI_PHASE_2EDGE;
  _hspi.Init.NSS = SPI_NSS_SOFT;
  _hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;  //baud: 42000000HZ
  _hspi.Init.FirstBit = SPI_FIRSTBIT_MSB;
  _hspi.Init.TIMode = SPI_TIMODE_DISABLE;
  _hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  _hspi.Init.CRCPolynomial = 10;
  HAL_SPI_Init(&_hspi);
}

void XPT2046::calibrate()
{

}

void XPT2046::read()
{
  uint8_t buf;
  _csPin.reset();
  writeCmd(0x90);
  readDat(&buf, sizeof(buf));
  _point.x = buf << 8;
  readDat(&buf, sizeof(buf));
  _point.x |= buf;
  _point.x >>= 3;
  writeCmd(0xd0);
  readDat(&buf, sizeof(buf));
  _point.y = buf << 8;
  readDat(&buf, sizeof(buf));
  _point.y |= buf;
  _point.y >>= 3;
  _csPin.set();
}

void XPT2046::writeCmd(uint8_t cmd)
{
   HAL_SPI_Transmit(&_hspi, &cmd, 1, 0xffff);
}

void XPT2046::readDat(uint8_t *buf, uint16_t len)
{
  HAL_SPI_Transmit(&_hspi, buf, len, 0xffff);
}
