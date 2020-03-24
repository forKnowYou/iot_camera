#include "ili9341.h"

ILI9341 ili(Pin(GPIOB, GPIO_PIN_15), 4, Pin(GPIOF, GPIO_PIN_12), 6, 240, 320);

void HAL_SRAM_MspInit(SRAM_HandleTypeDef* sramHandle)
{
  if(sramHandle == &ili._hsram)
    ili.gpioInit();
}

ILI9341::ILI9341(Pin bkLedPin, uint16_t nex, Pin cdPin, uint16_t cdOffset, uint16_t width, uint16_t height):
  _bkLedPin(bkLedPin), _cdPin(cdPin), _nex(nex), _cdOffset(cdOffset), _rowWidth(width), _rowHeight(_height), _width(width), _height(height)
{

}

void ILI9341::begin()
{
  _bkLedPin.setOut(Pin::high, Pin::pp);
  bkLedON();

  FSMC_NORSRAM_TimingTypeDef Timing = {0};

  /** Perform the SRAM1 memory initialization sequence
  */
  _hsram.Instance = FSMC_NORSRAM_DEVICE;
  _hsram.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;
  /* hsram.Init */
  switch(_nex) {
    case 1: _hsram.Init.NSBank = FSMC_NORSRAM_BANK1; _cmdAddr = 0x60000000; break;
    case 2: _hsram.Init.NSBank = FSMC_NORSRAM_BANK2; _cmdAddr = 0x64000000; break;
    case 3: _hsram.Init.NSBank = FSMC_NORSRAM_BANK3; _cmdAddr = 0x68000000; break;
    case 4: _hsram.Init.NSBank = FSMC_NORSRAM_BANK4; _cmdAddr = 0x6c000000; break;
    default: return;
  }
  _datAddr = _cmdAddr | (0x02 << _cdOffset);
  _hsram.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;
  _hsram.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;
  _hsram.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;
  _hsram.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
  _hsram.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
  _hsram.Init.WrapMode = FSMC_WRAP_MODE_DISABLE;
  _hsram.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
  _hsram.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
  _hsram.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
  _hsram.Init.ExtendedMode = FSMC_EXTENDED_MODE_DISABLE;
  _hsram.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
  _hsram.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;
  _hsram.Init.PageSize = FSMC_PAGE_SIZE_NONE;
  /* Timing */
  Timing.AddressSetupTime = 3;
  Timing.AddressHoldTime = 3;
  Timing.DataSetupTime = 3;
  Timing.BusTurnAroundDuration = 3;
  Timing.CLKDivision = 2;
  Timing.DataLatency = 2;
  Timing.AccessMode = FSMC_ACCESS_MODE_A;
  /* ExtTiming */

  HAL_SRAM_Init(&_hsram, &Timing, NULL);

  uart1Print.print("fsmc cmd and dat addr: ");
  uart1Print.print(_cmdAddr, 16);
  uart1Print.print(" ");
  uart1Print.print(_datAddr, 16);
  uart1Print.println();
  uart1Print.print("read lcd id: 0x");
  writeCmd(0xd3);
  _id = readDat();
  _id = readDat();
  _id = readDat();
  _id <<= 8;
  _id |= readDat();

  uart1Print.println((int32_t) _id, 16);

  if(_id != 0x9341) {
    uart1Print.println("lcd id error, system stop.");
    while(1);
  }

  writeCmd(0xCF);
  writeDat(0x00);
  writeDat(0xC1);
  writeDat(0X30);
  writeCmd(0xED);
  writeDat(0x64);
  writeDat(0x03);
  writeDat(0X12);
  writeDat(0X81);
  writeCmd(0xE8);
  writeDat(0x85);
  writeDat(0x10);
  writeDat(0x7A);
  writeCmd(0xCB);
  writeDat(0x39);
  writeDat(0x2C);
  writeDat(0x00);
  writeDat(0x34);
  writeDat(0x02);
  writeCmd(0xF7);
  writeDat(0x20);
  writeCmd(0xEA);
  writeDat(0x00);
  writeDat(0x00);
  writeCmd(0xC0);    //Power control
  writeDat(0x1B);   //VRH[5:0]
  writeCmd(0xC1);    //Power control
  writeDat(0x01);   //SAP[2:0];BT[3:0]
  writeCmd(0xC5);    //VCM control
  writeDat(0x30); 	 //3F
  writeDat(0x30); 	 //3C
  writeCmd(0xC7);    //VCM control2
  writeDat(0XB7);
  writeCmd(0x36);    // Memory Access Control
  writeDat(0x48);
  writeCmd(0x3A);
  writeDat(0x55);
  writeCmd(0xB1);
  writeDat(0x00);
  writeDat(0x1A);
  writeCmd(0xB6);    // Display Function Control
  writeDat(0x0A);
  writeDat(0xA2);
  writeCmd(0xF2);    // 3Gamma Function Disable
  writeDat(0x00);
  writeCmd(0x26);    //Gamma curve selected
  writeDat(0x01);
  writeCmd(0xE0);    //Set Gamma
  writeDat(0x0F);
  writeDat(0x2A);
  writeDat(0x28);
  writeDat(0x08);
  writeDat(0x0E);
  writeDat(0x08);
  writeDat(0x54);
  writeDat(0XA9);
  writeDat(0x43);
  writeDat(0x0A);
  writeDat(0x0F);
  writeDat(0x00);
  writeDat(0x00);
  writeDat(0x00);
  writeDat(0x00);
  writeCmd(0XE1);    //Set Gamma
  writeDat(0x00);
  writeDat(0x15);
  writeDat(0x17);
  writeDat(0x07);
  writeDat(0x11);
  writeDat(0x06);
  writeDat(0x2B);
  writeDat(0x56);
  writeDat(0x3C);
  writeDat(0x05);
  writeDat(0x10);
  writeDat(0x0F);
  writeDat(0x3F);
  writeDat(0x3F);
  writeDat(0x0F);
  writeCmd(0x2B);
  writeDat(0x00);
  writeDat(0x00);
  writeDat(0x01);
  writeDat(0x3f);
  writeCmd(0x2A);
  writeDat(0x00);
  writeDat(0x00);
  writeDat(0x00);
  writeDat(0xef);
  writeCmd(0x11); //Exit Sleep
  delayTimer.delayMs(120);
  writeCmd(0x29); //display on
}

void ILI9341::gpioInit()
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  __HAL_RCC_FSMC_CLK_ENABLE();

  /** FSMC GPIO Configuration
  PF12   ------> FSMC_A6
  PE7   ------> FSMC_D4
  PE8   ------> FSMC_D5
  PE9   ------> FSMC_D6
  PE10   ------> FSMC_D7
  PE11   ------> FSMC_D8
  PE12   ------> FSMC_D9
  PE13   ------> FSMC_D10
  PE14   ------> FSMC_D11
  PE15   ------> FSMC_D12
  PD8   ------> FSMC_D13
  PD9   ------> FSMC_D14
  PD10   ------> FSMC_D15
  PD14   ------> FSMC_D0
  PD15   ------> FSMC_D1
  PD0   ------> FSMC_D2
  PD1   ------> FSMC_D3
  PD4   ------> FSMC_NOE
  PD5   ------> FSMC_NWE
  PG12   ------> FSMC_NE4
  */
  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = _cdPin._pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;

  HAL_GPIO_Init(_cdPin._gpiox, &GPIO_InitStruct);

  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;

  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_14
                          |GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4
                          |GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;

  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* GPIO_InitStruct */
  GPIO_TypeDef *gpiox;
  switch(_nex) {
    case 1: GPIO_InitStruct.Pin = GPIO_PIN_7; gpiox = GPIOD; break;
    case 2: GPIO_InitStruct.Pin = GPIO_PIN_9; gpiox = GPIOG; break;
    case 3: GPIO_InitStruct.Pin = GPIO_PIN_10; gpiox = GPIOG; break;
    case 4: GPIO_InitStruct.Pin = GPIO_PIN_12; gpiox = GPIOG; break;
    default: return;
  }
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;

  HAL_GPIO_Init(gpiox, &GPIO_InitStruct);
}

void ILI9341::setCursor(uint16_t xs, uint16_t xe, uint16_t ys, uint16_t ye)
{
  writeCmd(0x2a);
  writeDat(xs >> 8);
  writeDat(xs & 0xff);
  writeDat(xe >> 8);
  writeDat(xe & 0xff);
  writeCmd(0x2b);
  writeDat(ys >> 8);
  writeDat(ys & 0xff);
  writeDat(ye >> 8);
  writeDat(ye & 0xff);
}

void ILI9341::setWriteToGram()
{
  writeCmd(0x2c);
}

void ILI9341::setScanRotation(rotation_t rot)
{
  uint16_t val = 0;
  switch(rot) {
    case rotation0: val = 0; _width = _rowWidth; _height = _rowHeight; break;
    case rotation90: val = 0x20; _width = _rowHeight; _height = _rowWidth; break;
    default: return;
  }
  writeCmd(0x36);
  writeDat(val);
}

void ILI9341::fillRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color)
{
  if((!w) || (!h))
    return;
  uint16_t xs = x0, xe = x0 + w - 1;
  uint16_t ys = y0, ye = y0 + h - 1;
  switch(_rotation) {
    case rotation90: ys = _height - ye; ye = _height - y0; break;
    case rotation180: xs = _width - xe; xe = _width - x0; ys = _height - ye; ye = _height - y0; break;
    case rotation270: xs = _width - xe; xe = _width - x0; break;
  }

  writeCmd(0x2a);
  writeDat(xs >> 8);
  writeDat(xs & 0xff);
  writeDat(xe >> 8);
  writeDat(xe & 0xff);
  writeCmd(0x2b);
  writeDat(ys >> 8);
  writeDat(ys & 0xff);
  writeDat(ye >> 8);
  writeDat(ye & 0xff);
  writeCmd(0x2c);
  for(uint16_t i = 0; i < w; i ++) {
    for(uint16_t j = 0; j < h; j ++)
      writeDat(color);
  }
}

void ILI9341::bkLedON()
{
  _bkLedPin.set();
}

void ILI9341::bkLedOFF()
{
  _bkLedPin.reset();
}

void ILI9341::setRotation(rotation_t rot)
{
  _rotation = rot;
}

void ILI9341::writeCmd(uint16_t cmd)
{
  *(uint16_t*) _cmdAddr = cmd;
}

void ILI9341::writeDat(uint16_t dat)
{
  *(uint16_t*) _datAddr = dat;
}

void ILI9341::writeDatBuffer(uint16_t *buf, uint32_t len)
{
  while(len --) {
    *(uint16_t*) _datAddr = *buf;
    buf ++;
  }
}

uint16_t ILI9341::readDat()
{
  return *(uint16_t*) _datAddr;
}
