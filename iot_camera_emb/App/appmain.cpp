#include "appmain.h"
#include "print.h"
#include "ili9341.h"
#include "timer.h"
#include "gui.h"
#include "xpt2046.h"
#include "ov7725.h"
#include "ledApp.h"
#include "espApp.h"

void PrintBegin()
{
  uart1Print.begin(2000000);
}

void GuiFillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, color_t color)
{
  ili.fillRect(x, y, w, h, (uint16_t) color);
}

Button btShowCamera("btShowCamera", 40, 260, 160, 40, 16, 0, "show camera");
uint8_t taConsoleBuffer[512] = {0};
TextArea taConsole("taConsole", 0, 0, 240, 240, 0, taConsoleBuffer, sizeof(taConsoleBuffer));

Widget *mainPageWidgets[] = {
  &btShowCamera, &taConsole
};

Page pages[] = {
  Page("main", mainPageWidgets, sizeof(mainPageWidgets) / sizeof(mainPageWidgets[0]), Color::navy),
  Page("camera", 0, 0, Color::black)
};

GUI gui;

void OvI2cDelay()
{
  delayTimer.delayUs(2);
}

void OvReadDataInit()
{
  GPIO_InitTypeDef init;
  init.Mode = GPIO_MODE_INPUT;
  init.Pull = GPIO_PULLUP;
  init.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
  HAL_GPIO_Init(GPIOC, &init);
}

#define OV_WINDOW_WIDTH   320
#define OV_WINDOW_HEIGHT  240

void OvWriteFrameStart()
{
  ili.setScanRotation(ili.rotation90);
  ili.setCursor(0, 320, 0, 240);
  ili.setWriteToGram();
}

void OvWriteFrameEnd()
{
  ili.setScanRotation(ili.rotation0);
  ili.setCursor(0, 240, 0, 320);
}

OV7725 ov(0x42 >> 1, // addr
          SoftIIC(Pin(GPIOF, GPIO_PIN_1), Pin(GPIOF, GPIO_PIN_0), OvI2cDelay), // iic
          Pin(GPIOC, GPIO_PIN_9),  // fifoWen
          Pin(GPIOA, GPIO_PIN_8),  // fifoWrst
          Pin(GPIOC, GPIO_PIN_10),  // fifoRrst
          Pin(GPIOC, GPIO_PIN_11),  // fifoOe
          Pin(GPIOC, GPIO_PIN_12),  // vsync
          GPIOC, GPIO_PIN_8, GPIOC, 0xff, 0x6c000080
);

uint16_t ovFrameBuffer[OV_WINDOW_WIDTH * 8] = {0};

void AppMain(void)
{
  uint8_t temp8;
  uint32_t temp32;

  delayTimer.begin();
  delayTimer.delayMs(50);
  PrintBegin();
  LedAppInit();
  EspAppInit();

  uart1Print.println();
  uart1Print.println("system init ...");
  ili.begin();
  ili.setRotation(ili.rotation90);
  uart1Print.print("flush screen test: ");
  temp32 = HAL_GetTick();
  ili.fillRect(0, 0, 240, 320, Color::black);
  uart1Print.println(HAL_GetTick() - temp32);

  gui.begin(GuiFillRect, 240, 320, pages, sizeof(pages) / sizeof(pages[0]));
  taConsole._background = Color::maroon;
  gui.showPage("main");
  taConsole.println("screen init success");
  taConsole.show();

  if(! ov.begin()) {
    taConsole.println("ov7725 init fail!");
    taConsole.show();
  } else {
    taConsole.println("ov7725 init success!");
    taConsole.show();
    OvReadDataInit();
    // ov.setSingleFrame(true);
    ov.setLightMode(OV7725::lightModeAuto);
    ov.setSaturation(0);
    ov.setBrightness(0);
    ov.setContrast(0);
    ov.setSpacialEffects(OV7725::spacialEffectNormal);
    ov.setWindow(OV_WINDOW_WIDTH, OV_WINDOW_HEIGHT, OV7725::resolutionQvga);

    taConsole.print("ov7725 com11 value: ");
    ov.readReg(OV7725::reg_COM11, &temp8);
    taConsole.println(temp8, 16);
    taConsole.show();

    if(0) {  // test camera, loop forever
      HAL_Delay(3200);
      uart1Print.print("get frame test: ");
      OvWriteFrameStart();
      ov.trigSingleFrame();
      ov.getFrameBufferStart(OV_WINDOW_WIDTH, OV_WINDOW_HEIGHT);
      temp32 = HAL_GetTick();
      while(ov.getFrameBufferAvailable()) {
        ov.getFrameBuffer(8, ovFrameBuffer);
        ili.writeDatBuffer(ovFrameBuffer, sizeof(ovFrameBuffer) / sizeof(ovFrameBuffer[0]));
      }
      OvWriteFrameEnd();
      uart1Print.println(HAL_GetTick() - temp32);
      while(1) {
        OvWriteFrameStart();
        ov.trigSingleFrame();
        ov.getFrameBufferStart(OV_WINDOW_WIDTH, OV_WINDOW_HEIGHT);
        while(ov.getFrameBufferAvailable()) {
          ov.getFrameBuffer(8, ovFrameBuffer);
          ili.writeDatBuffer(ovFrameBuffer, sizeof(ovFrameBuffer) / sizeof(ovFrameBuffer[0]));
        }
        OvWriteFrameEnd();
      }
    }
  }

  HAL_Delay(2000);
  taConsole.println("ready to enter task scheduler");
  taConsole.show();
}
