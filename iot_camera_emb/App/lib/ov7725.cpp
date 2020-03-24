#include "ov7725.h"

const uint8_t initTable[][2] = {
  {OV7725::reg_CLKRC,     0x00}, //clock config
  {OV7725::reg_COM7,      0x46}, //QVGA RGB565
  {OV7725::reg_HSTART,    0x3f},
  {OV7725::reg_HSIZE,     0x50},
  {OV7725::reg_VSTRT,     0x03},
  {OV7725::reg_VSIZE,     0x78},
  {OV7725::reg_HREF,      0x00},
  {OV7725::reg_HOutSize,  0x50},
  {OV7725::reg_VOutSize,  0x78},

  /*DSP control*/
  {OV7725::reg_TGT_B,     0x7f},
  {OV7725::reg_FixGain,   0x09},
  {OV7725::reg_AWB_Ctrl0, 0xe0},
  {OV7725::reg_DSP_Ctrl1, 0xff},
  {OV7725::reg_DSP_Ctrl2, 0x00},
  {OV7725::reg_DSP_Ctrl3,	0x00},
  {OV7725::reg_DSP_Ctrl4, 0x00},

  /*AGC AEC AWB*/
  {OV7725::reg_COM8,		0xf0},
  {OV7725::reg_COM4,		0x81}, /*Pll AEC CONFIG*/
  {OV7725::reg_COM6,		0xc5},
  {OV7725::reg_COM9,		0x11},
  {OV7725::reg_BDBase,	0x7F},
  {OV7725::reg_BDMStep,	0x03},
  {OV7725::reg_AEW,		0x40},
  {OV7725::reg_AEB,		0x30},
  {OV7725::reg_VPT,		0xa1},
  {OV7725::reg_EXHCL,		0x9e},
  {OV7725::reg_AWBCtrl3,  0xaa},
  {OV7725::reg_COM8,		0xff},

  /*matrix shapness brightness contrast*/
  {OV7725::reg_EDGE1,		0x08},
  {OV7725::reg_DNSOff,	0x01},
  {OV7725::reg_EDGE2,		0x03},
  {OV7725::reg_EDGE3,		0x00},
  {OV7725::reg_MTX1,		0xb0},
  {OV7725::reg_MTX2,		0x9d},
  {OV7725::reg_MTX3,		0x13},
  {OV7725::reg_MTX4,		0x16},
  {OV7725::reg_MTX5,		0x7b},
  {OV7725::reg_MTX6,		0x91},
  {OV7725::reg_MTX_Ctrl,  0x1e},
  {OV7725::reg_BRIGHT,	0x08},
  {OV7725::reg_CNST,		0x20},
  {OV7725::reg_UVADJ0,	0x81},
  {OV7725::reg_SDE, 		0X06},
  {OV7725::reg_USAT,		0x65},
  {OV7725::reg_VSAT,		0x65},
  {OV7725::reg_HUECOS,	0X80},
  {OV7725::reg_HUESIN, 	0X80},

  /*GAMMA config*/
  {OV7725::reg_GAM1,		0x0c},
  {OV7725::reg_GAM2,		0x16},
  {OV7725::reg_GAM3,		0x2a},
  {OV7725::reg_GAM4,		0x4e},
  {OV7725::reg_GAM5,		0x61},
  {OV7725::reg_GAM6,		0x6f},
  {OV7725::reg_GAM7,		0x7b},
  {OV7725::reg_GAM8,		0x86},
  {OV7725::reg_GAM9,		0x8e},
  {OV7725::reg_GAM10,		0x97},
  {OV7725::reg_GAM11,		0xa4},
  {OV7725::reg_GAM12,		0xaf},
  {OV7725::reg_GAM13,		0xc5},
  {OV7725::reg_GAM14,		0xd7},
  {OV7725::reg_GAM15,		0xe8},
  {OV7725::reg_SLOP,		0x20},


  {OV7725::reg_COM3,		0x50},/*Horizontal mirror image*/
  /*night mode auto frame rate control*/
  {OV7725::reg_COM5,		0xf5},
  //{COM5,		0x31},
};

const uint16_t intiTableLen = sizeof(initTable) / sizeof(initTable[0]);

OV7725::OV7725(uint8_t addr, SoftIIC iic,
               Pin fifoWen, Pin fifoWrst, Pin fifoRrst, Pin fifoOe, Pin vsync,
               GPIO_TypeDef *rclkPort, uint32_t rclkPin, GPIO_TypeDef *datPort, uint32_t datPin,
               uint32_t pixelAddr) :
               _addr(addr), _iic(iic),
               _fifoWen(fifoWen), _fifoWrst(fifoWrst), _fifoRrst(fifoRrst), _fifoOe(fifoOe), _vsync(vsync),
               _rclkPort(rclkPort), _rclkPin(rclkPin), _datPort(datPort), _datPin(datPin),
               _pixelAddr(pixelAddr)
{
  _getFrameBufferW = 0;
  _getFrameBufferH = 0;
}

bool OV7725::begin()
{
  uint8_t buf;

  _iic.begin();
  Pin rclkPin(_rclkPort, _rclkPin);
  rclkPin.setOut(Pin::high, Pin::pp);
  _fifoWen.setOut(Pin::high, Pin::pp);
  _fifoWrst.setOut(Pin::high, Pin::pp);
  _fifoRrst.setOut(Pin::high, Pin::pp);
  _fifoOe.setOut(Pin::high, Pin::pp);
  _vsync.setInput(Pin::pullUp);
  _rclkPort->ODR |= _rclkPin;
  _fifoWen.set();
  _fifoWrst.set();
  _fifoRrst.set();
  _fifoOe.set();

  if(! readReg(0x0a, &buf))
    return false;
  if(buf != 0x77)
    return false;
  readReg(0x0b, &buf);
  if(buf != 0x21)
    return false;
  writeReg(0x12, 0x80);  // reset register
  for(uint16_t i = 0; i < intiTableLen; i ++)
    writeReg(initTable[i][0], initTable[i][1]);
  _fifoOe.reset();
  return true;
}

void OV7725::setSingleFrame(bool on)
{
  if(on)
    writeReg(reg_COM11, 0x02);
  else
    writeReg(reg_COM11, 0x00);
}

void OV7725::trigSingleFrame()
{
  // writeReg(reg_COM11, 0x03);
  while(! _vsync.read());
  while(_vsync.read());
  _fifoWrst.reset();
  _rclkPort->ODR &= ~_rclkPin;
  _rclkPort->ODR |= _rclkPin;
  _fifoWrst.set();
  _fifoWen.set();
  while(! _vsync.read());
  _fifoWen.reset();

//  _fifoWrst.reset();
//  _rclkPort->ODR &= ~_rclkPin;
//  _rclkPort->ODR |= _rclkPin;
//  _fifoWrst.set();
//  _fifoWen.set();
//  writeReg(reg_COM11, 0x03);
//  while(! _vsync.read());
//  _fifoWen.reset();
}

void OV7725::getSingleFrame(uint16_t w, uint16_t h, funWriteFramePixel fWrite)
{
  _fifoRrst.reset();
  _rclkPort->ODR &= ~_rclkPin;
  _rclkPort->ODR |= _rclkPin;
  _rclkPort->ODR &= ~_rclkPin;
  _fifoRrst.set();
  _rclkPort->ODR |= _rclkPin;

  uint16_t dat;
  if(fWrite) {
    for(uint16_t i = 0; i < h; i ++) {
      for(uint16_t j = 0; j < w; j ++) {
        _rclkPort->ODR &= ~_rclkPin;
        dat = _datPort->IDR & _datPin;
        dat <<= 8;
        _rclkPort->ODR |= _rclkPin;
        _rclkPort->ODR &= ~_rclkPin;
        dat |= _datPort->IDR & _datPin;
        _rclkPort->ODR |= _rclkPin;
        fWrite(dat);
      }
    }
  } else {
    for(uint16_t i = 0; i < h; i ++) {
      for(uint16_t j = 0; j < w; j ++) {
        _rclkPort->ODR &= ~_rclkPin;
        dat = _datPort->IDR & _datPin;
        dat <<= 8;
        _rclkPort->ODR |= _rclkPin;
        _rclkPort->ODR &= ~_rclkPin;
        dat |= _datPort->IDR & _datPin;
        _rclkPort->ODR |= _rclkPin;
        *(uint16_t*) _pixelAddr = dat;
      }
    }
  }
}

void OV7725::getFrameBufferStart(uint16_t w, uint16_t h)
{
  _getFrameBufferW = w;
  _getFrameBufferH = h;

  _fifoRrst.reset();
  _rclkPort->ODR &= ~_rclkPin;
  _rclkPort->ODR |= _rclkPin;
  _rclkPort->ODR &= ~_rclkPin;
  _fifoRrst.set();
  _rclkPort->ODR |= _rclkPin;
}

bool OV7725::getFrameBufferAvailable()
{
  return _getFrameBufferH ? true : false;
}

void OV7725::getFrameBuffer(uint16_t h, uint16_t *buf)
{
  if(h > _getFrameBufferH)
    h = _getFrameBufferH;
  _getFrameBufferH -= h;
  for(uint16_t i = 0; i < h; i ++) {
    for(uint16_t j = 0; j < _getFrameBufferW; j ++) {
      _rclkPort->ODR &= ~_rclkPin;
      *buf = _datPort->IDR & _datPin;
      *buf <<= 8;
      _rclkPort->ODR |= _rclkPin;
      _rclkPort->ODR &= ~_rclkPin;
      *buf |= _datPort->IDR & _datPin;
      _rclkPort->ODR |= _rclkPin;
      buf ++;
    }
  }
}

void OV7725::setLightMode(lightMode_t e)
{
  switch(e) {
    case lightModeAuto:	//Auto
      writeReg(0x13, 0xff); //AWB on
      writeReg(0x0e, 0x65);
      writeReg(0x2d, 0x00);
      writeReg(0x2e, 0x00);
      break;
    case lightModeSunny://sunny
      writeReg(0x13, 0xfd); //AWB off
      writeReg(0x01, 0x5a);
      writeReg(0x02, 0x5c);
      writeReg(0x0e, 0x65);
      writeReg(0x2d, 0x00);
      writeReg(0x2e, 0x00);
      break;
    case lightModeCloudy://cloudy
      writeReg(0x13, 0xfd); //AWB off
      writeReg(0x01, 0x58);
      writeReg(0x02, 0x60);
      writeReg(0x0e, 0x65);
      writeReg(0x2d, 0x00);
      writeReg(0x2e, 0x00);
      break;
    case lightModeOffice://office
      writeReg(0x13, 0xfd); //AWB off
      writeReg(0x01, 0x84);
      writeReg(0x02, 0x4c);
      writeReg(0x0e, 0x65);
      writeReg(0x2d, 0x00);
      writeReg(0x2e, 0x00);
      break;
    case lightModeHome://home
      writeReg(0x13, 0xfd); //AWB off
      writeReg(0x01, 0x96);
      writeReg(0x02, 0x40);
      writeReg(0x0e, 0x65);
      writeReg(0x2d, 0x00);
      writeReg(0x2e, 0x00);
      break;
    case lightModeNight://night
      writeReg(0x13, 0xff); //AWB on
      writeReg(0x0e, 0xe5);
      break;
  }
}

void OV7725::setSaturation(int8_t sat)
{
  if(sat >= -4 && sat <= 4) {
    writeReg(reg_USAT,(sat+4)<<4);
    writeReg(reg_VSAT,(sat+4)<<4);
  }
}

void OV7725::setBrightness(int8_t bri)
{
  uint8_t value = 0x08, sign;
  if(bri >= -4 && bri <= 4) {
    if(bri >= 0) {
      sign = 0x06;
    } else {
      sign = 0x0e;
      bri = -(bri + 1);
    }
    while(bri --)
      value += 0x10;
    writeReg(reg_BRIGHT, value);
    writeReg(reg_SIGN, sign);
  }
}

void OV7725::setContrast(int8_t con)
{
  if(con >= -4 && con <= 4) {
    writeReg(reg_CNST, 0x30 - (4 - con) * 4);
  }
}

void OV7725::setSpacialEffects(spacialEffect_t e)
{
  switch(e) {
    case spacialEffectNormal:
      writeReg(0xa6, 0x06);
      writeReg(0x60, 0x80);
      writeReg(0x61, 0x80);
      break;
  }
}

void OV7725::setWindow(uint16_t w, uint16_t h, resolution_t res)
{
  uint8_t raw,temp;
  uint16_t sx,sy;

  if(res == resolutionVga)
  {
    sx=(640-w)/2;
    sy=(480-h)/2;
    writeReg(reg_COM7,0x06);
    writeReg(reg_HSTART,0x23);
    writeReg(reg_HSIZE,0xA0);
    writeReg(reg_VSTRT,0x07);
    writeReg(reg_VSIZE,0xF0);
    writeReg(reg_HREF,0x00);
    writeReg(reg_HOutSize,0xA0);
    writeReg(reg_VOutSize,0xF0);
  }
  else
  {
    sx=(320-w)/2;
    sy=(240-h)/2;
    writeReg(reg_COM7,0x46);
    writeReg(reg_HSTART,0x3f);
    writeReg(reg_HSIZE, 0x50);
    writeReg(reg_VSTRT, 0x03);
    writeReg(reg_VSIZE, 0x78);
    writeReg(reg_HREF,  0x00);
    writeReg(reg_HOutSize,0x50);
    writeReg(reg_VOutSize,0x78);
  }
  readReg(reg_HSTART, &raw);
  temp=raw+(sx>>2);
  writeReg(reg_HSTART,temp);
  writeReg(reg_HSIZE,w>>2);

  readReg(reg_VSTRT, &raw);
  temp=raw+(sy>>1);
  writeReg(reg_VSTRT,temp);
  writeReg(reg_VSIZE,h>>1);

  readReg(reg_HREF, &raw);
  temp=((sy&0x01)<<6)|((sx&0x03)<<4)|((h&0x01)<<2)|(w&0x03)|raw;
  writeReg(reg_HREF,temp);

  writeReg(reg_HOutSize,w>>2);
  writeReg(reg_VOutSize,h>>1);

  readReg(reg_EXHCH, &temp);
  temp = (raw|(w&0x03)|((h&0x01)<<2));
  writeReg(reg_EXHCH,temp);
}

bool OV7725::writeReg(uint8_t reg, uint8_t dat)
{
  return _iic.write(_addr, reg, &dat, 1);
}

bool OV7725::readReg(uint8_t reg, uint8_t *buf)
{
  return _iic.read(_addr, reg, buf, 1);
}
