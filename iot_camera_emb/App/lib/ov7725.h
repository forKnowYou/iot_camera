#ifndef OV7725_H
#define OV7725_H

#include "softiic.h"
#include "timer.h"
#include "print.h"

class OV7725 {
public:
  typedef enum {
    reg_GAIN      = 0x00,
    reg_BLUE      = 0x01,
    reg_RED       = 0x02,
    reg_GREEN     = 0x03,
    reg_BAVG      = 0x05,
    reg_GAVG      = 0x06,
    reg_RAVG      = 0x07,
    reg_AECH      = 0x08,
    reg_COM2      = 0x09,
    reg_PID       = 0x0A,
    reg_VER       = 0x0B,
    reg_COM3      = 0x0C,
    reg_COM4      = 0x0D,
    reg_COM5      = 0x0E,
    reg_COM6      = 0x0F,
    reg_AEC       = 0x10,
    reg_CLKRC     = 0x11,
    reg_COM7      = 0x12,
    reg_COM8      = 0x13,
    reg_COM9      = 0x14,
    reg_COM10     = 0x15,
    reg_REG16     = 0x16,
    reg_HSTART    = 0x17,
    reg_HSIZE     = 0x18,
    reg_VSTRT     = 0x19,
    reg_VSIZE     = 0x1A,
    reg_PSHFT     = 0x1B,
    reg_MIDH      = 0x1C,
    reg_MIDL      = 0x1D,
    reg_LAEC      = 0x1F,
    reg_COM11     = 0x20,
    reg_BDBase    = 0x22,
    reg_BDMStep   = 0x23,
    reg_AEW       = 0x24,
    reg_AEB       = 0x25,
    reg_VPT       = 0x26,
    reg_REG28     = 0x28,
    reg_HOutSize  = 0x29,
    reg_EXHCH     = 0x2A,
    reg_EXHCL     = 0x2B,
    reg_VOutSize  = 0x2C,
    reg_ADVFL     = 0x2D,
    reg_ADVFH     = 0x2E,
    reg_YAVE      = 0x2F,
    reg_LumHTh    = 0x30,
    reg_LumLTh    = 0x31,
    reg_HREF      = 0x32,
    reg_DM_LNL    = 0x33,
    reg_DM_LNH    = 0x34,
    reg_ADoff_B   = 0x35,
    reg_ADoff_R   = 0x36,
    reg_ADoff_Gb  = 0x37,
    reg_ADoff_Gr  = 0x38,
    reg_Off_B     = 0x39,
    reg_Off_R     = 0x3A,
    reg_Off_Gb    = 0x3B,
    reg_Off_Gr    = 0x3C,
    reg_COM12     = 0x3D,
    reg_COM13     = 0x3E,
    reg_COM14     = 0x3F,
    reg_COM16     = 0x41,
    reg_TGT_B     = 0x42,
    reg_TGT_R     = 0x43,
    reg_TGT_Gb    = 0x44,
    reg_TGT_Gr    = 0x45,
    reg_LC_CTR    = 0x46,
    reg_LC_XC     = 0x47,
    reg_LC_YC     = 0x48,
    reg_LC_COEF   = 0x49,
    reg_LC_RADI   = 0x4A,
    reg_LC_COEFB  = 0x4B,
    reg_LC_COEFR  = 0x4C,
    reg_FixGain   = 0x4D,
    reg_AREF1     = 0x4F,
    reg_AREF6     = 0x54,
    reg_UFix      = 0x60,
    reg_VFix      = 0x61,
    reg_AWBb_blk  = 0x62,
    reg_AWB_Ctrl0 = 0x63,
    reg_DSP_Ctrl1 = 0x64,
    reg_DSP_Ctrl2 = 0x65,
    reg_DSP_Ctrl3 = 0x66,
    reg_DSP_Ctrl4 = 0x67,
    reg_AWB_bias  = 0x68,
    reg_AWBCtrl1  = 0x69,
    reg_AWBCtrl2  = 0x6A,
    reg_AWBCtrl3  = 0x6B,
    reg_AWBCtrl4  = 0x6C,
    reg_AWBCtrl5  = 0x6D,
    reg_AWBCtrl6  = 0x6E,
    reg_AWBCtrl7  = 0x6F,
    reg_AWBCtrl8  = 0x70,
    reg_AWBCtrl9  = 0x71,
    reg_AWBCtrl10 = 0x72,
    reg_AWBCtrl11 = 0x73,
    reg_AWBCtrl12 = 0x74,
    reg_AWBCtrl13 = 0x75,
    reg_AWBCtrl14 = 0x76,
    reg_AWBCtrl15 = 0x77,
    reg_AWBCtrl16 = 0x78,
    reg_AWBCtrl17 = 0x79,
    reg_AWBCtrl18 = 0x7A,
    reg_AWBCtrl19 = 0x7B,
    reg_AWBCtrl20 = 0x7C,
    reg_AWBCtrl21 = 0x7D,
    reg_GAM1      = 0x7E,
    reg_GAM2      = 0x7F,
    reg_GAM3      = 0x80,
    reg_GAM4      = 0x81,
    reg_GAM5      = 0x82,
    reg_GAM6      = 0x83,
    reg_GAM7      = 0x84,
    reg_GAM8      = 0x85,
    reg_GAM9      = 0x86,
    reg_GAM10     = 0x87,
    reg_GAM11     = 0x88,
    reg_GAM12     = 0x89,
    reg_GAM13     = 0x8A,
    reg_GAM14     = 0x8B,
    reg_GAM15     = 0x8C,
    reg_SLOP      = 0x8D,
    reg_DNSTh     = 0x8E,
    reg_EDGE0     = 0x8F,
    reg_EDGE1     = 0x90,
    reg_DNSOff    = 0x91,
    reg_EDGE2     = 0x92,
    reg_EDGE3     = 0x93,
    reg_MTX1      = 0x94,
    reg_MTX2      = 0x95,
    reg_MTX3      = 0x96,
    reg_MTX4      = 0x97,
    reg_MTX5      = 0x98,
    reg_MTX6      = 0x99,
    reg_MTX_Ctrl  = 0x9A,
    reg_BRIGHT    = 0x9B,
    reg_CNST      = 0x9C,
    reg_UVADJ0    = 0x9E,
    reg_UVADJ1    = 0x9F,
    reg_SCAL0     = 0xA0,
    reg_SCAL1     = 0xA1,
    reg_SCAL2     = 0xA2,
    reg_SDE       = 0xA6,
    reg_USAT      = 0xA7,
    reg_VSAT      = 0xA8,
    reg_HUECOS    = 0xA9,
    reg_HUESIN    = 0xAA,
    reg_SIGN      = 0xAB,
    reg_DSPAuto   = 0xAC
  } reg_t;

  OV7725(uint8_t addr, SoftIIC iic,
         Pin fifoWen, Pin fifoWrst, Pin fifoRrst, Pin fifoOe, Pin vsync,
         GPIO_TypeDef *rclkPort, uint32_t rclkPin, GPIO_TypeDef *datPort, uint32_t datPin,
         uint32_t pixelAddr = 0);

  bool begin();

  void setSingleFrame(bool on);
  void trigSingleFrame();

  typedef void (*funWriteFramePixel)(uint16_t dat);

  void getSingleFrame(uint16_t w, uint16_t h, funWriteFramePixel fWrite = 0);
  void getFrameBufferStart(uint16_t w, uint16_t h);
  bool getFrameBufferAvailable();
  void getFrameBuffer(uint16_t h, uint16_t *buf);

  typedef enum {
    lightModeAuto,
    lightModeSunny,
    lightModeCloudy,
    lightModeOffice,
    lightModeHome,
    lightModeNight
  } lightMode_t;

  void setLightMode(lightMode_t e);
  void setSaturation(int8_t sat);
  void setBrightness(int8_t bri);
  void setContrast(int8_t con);

  typedef enum {
    spacialEffectNormal
  } spacialEffect_t;

  void setSpacialEffects(spacialEffect_t e);

  typedef enum {
    resolutionVga,
    resolutionQvga
  } resolution_t;

  void setWindow(uint16_t w, uint16_t h, resolution_t res);

  bool writeReg(uint8_t reg, uint8_t dat);
  bool readReg(uint8_t reg, uint8_t *buf);

private:
  const uint8_t _addr;
  SoftIIC _iic;
  Pin _fifoWen, _fifoWrst, _fifoRrst, _fifoOe, _vsync;
  GPIO_TypeDef *_rclkPort;
  uint32_t _rclkPin;
  GPIO_TypeDef *_datPort;
  uint32_t _datPin;
  uint32_t _pixelAddr;
  uint16_t _getFrameBufferW, _getFrameBufferH;
};

#endif
