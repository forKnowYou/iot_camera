#include "softiic.h"

SoftIIC::SoftIIC(Pin scl, Pin sda, funDelay fDelay) :
                 _scl(scl), _sda(sda), _funDelay(fDelay)
{

}

void SoftIIC::begin()
{
  _scl.setOut(Pin::fast, Pin::od);
  _sda.setOut(Pin::fast, Pin::od);
  _scl.reset();
  _sda.reset();
}

bool SoftIIC::write(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
  start();
  writeByte(addr << 1);
  if(! checkRes())
    return false;
  writeByte(reg);
  if(! checkRes())
    return false;
  while(len --) {
    writeByte(*buf);
    buf ++;
    if(! checkRes())
      return false;
  }
  end();
  return true;
}

bool SoftIIC::read(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
  start();
  writeByte(addr << 1);
  if(! checkRes())
    return false;
  writeByte(reg);
  if(! checkRes())
    return false;
  end();
  start();
  writeByte((addr << 1) | 0x01);
  if(! checkRes())
    return false;
  while(len --) {
    *buf = readByte();
    buf ++;
    if(len == 0)
      sendRes(false);
    else
      sendRes(true);
  }
  end();
  return true;
}

void SoftIIC::start()
{
  _sda.set();
  _scl.set();
  _funDelay();
  _sda.reset();
  _funDelay();
  _scl.reset();
}

void SoftIIC::end()
{
  _sda.reset();
  _scl.set();
  _funDelay();
  _sda.set();
}

void SoftIIC::writeByte(uint8_t dat)
{
  for(uint8_t i = 0; i < 8; i ++) {
    if(dat & 0x80)
      _sda.set();
    else
      _sda.reset();
    dat <<= 1;
    _funDelay();
    _scl.set();
    _funDelay();
    _scl.reset();
  }
  _sda.set();
}

uint8_t SoftIIC::readByte()
{
  uint8_t dat = 0;

  for(uint8_t i = 0; i < 8; i ++) {
    _funDelay();
    _scl.set();
    dat <<= 1;
    if(_sda.read())
      dat |= 0x01;
    _funDelay();
    _scl.reset();
  }
  return dat;
}

void SoftIIC::sendRes(bool res)
{
  if(res)
    _sda.reset();
  else
    _sda.set();
  _funDelay();
  _scl.set();
  _funDelay();
  _scl.reset();
  _sda.set();
}

bool SoftIIC::checkRes()
{
  bool res;

  _funDelay();
  _scl.set();
  if(_sda.read())
    res = false;
  else
    res = true;
  _funDelay();
  _scl.reset();
  return res;
}
