#include "espApp.h"
#include "ov7725.h"
#include "tinylibjpeg.h"
#include "tje.h"

char espBuf[512] = {0};
EspAt esp(UartPrint(USART1), espBuf, sizeof(espBuf), 4000000, HAL_GetTick, Pin(GPIOA, GPIO_PIN_4));

osThreadId espAppId;

const char stationName[] = "iot_camera_wifi";
const char stationPwd[] = "88888888";
const uint16_t cipServePort = 10500;

const char serverIp[] = "120.78.183.26";
const uint16_t serverPort = 8100;

char apName[32] = {0};
char apPwd[32] = {0};
bool haveAp = false;

const uint8_t espTestData[] = {0x30, 0x32, 0x32, 0x00, 0x31, 0x39, 0x38};

void EspCipReceive(const char *p, uint16_t len)
{
  taConsole.print("EspCipReceive(");
  taConsole.print(len);
  taConsole.println("): ");
  taConsole.println(p);
  taConsole.show();

  const char *pt = strstr(p, "+WIFI:");
  uint16_t i = 0;
  if(pt) {
    pt += sizeof("+WIFI:") - 1;
    while(*pt != ',') {
      apName[i] = *pt;
      i ++;
      pt ++;
    }
    i = 0;
    pt ++;
    while(*pt) {
      apPwd[i] = *pt;
      i ++;
      pt ++;
    }
    haveAp = true;
  }
}

bool EspInitSteps()
{
  static uint16_t step = 0;

  switch(step) {
    case 0: esp.setWifiMode(EspAt::wifiMode_staAndAp); step ++; break;
    case 1: {
      taConsole.print("wifiName: ");
      taConsole.println(stationName);
      taConsole.print("wifiPwd: ");
      taConsole.println(stationPwd);
      taConsole.show();
      esp.setSoftAp(stationName, stationPwd);
      step ++;
    } break;
    case 2: {
      taConsole.println("get wifi ip ...");
      taConsole.show();
      esp.getSoftApIp();
      step ++;
    } break;
    case 3: return esp.haveApIp ? true : false;
  }
  return false;
}

bool EspCreateServerSteps()
{
  static uint16_t step = 0;

  switch(step) {
    case 0: {
      taConsole.println("station ip not valid, wait for staion");
      taConsole.show();
      esp.setCipmux(true);
      step ++;
    } break;
    case 1: {
      taConsole.print("APIP:");
      taConsole.println((const char*) esp.apIp);
      taConsole.print("cipServePort: ");
      taConsole.println(cipServePort);
      taConsole.show();
      esp.setCipserve(true, cipServePort);
      step ++;
    } break;
    case 2: {
      if(haveAp) {
        taConsole.print("station connect to: ");
        taConsole.print(apName);
        taConsole.print(",");
        taConsole.println(apPwd);
        taConsole.show();
        esp.setStation((const char*) apName, (const char*) apPwd);
        haveAp = false;
      } else if((HAL_GetTick() - esp.lastCmdTick) > 4000) {
        esp.getSoftApIp();
      }
    } break;
  }
  return false;
}

uint16_t jpegCount;
uint16_t jpegId;
uint8_t jpegBuf[1472] = {0};
uint16_t jpegBufPoint = 0;
uint32_t jpegSendTick = 0;
uint32_t jpegDataTotal = 0;
bool showFirstImage = false;

#define JPEG_BUFFER_INTERVAL  22

void showBuf(uint8_t *buf, uint32_t len)
{
//  uart1Print.println();
//  while(len --) {
//    if(*buf < 16)
//      uart1Print.print("0");
//    uart1Print.print(*buf, 16);
//    uart1Print.print(" ");
//    buf ++;
//  }
//  uart1Print.println();
}

void writeJpegMsgHead()
{
  uint16_t temp = 0;
  memset(jpegBuf, 0, sizeof(jpegBuf));
  strcpy((char*) jpegBuf, "JPEG:");
  jpegBuf[5] = '0';
  jpegBuf[6] = '0';
  jpegBuf[7] = ',';
  jpegBuf[8] = '0';
  jpegBuf[9] = '0';
  jpegBuf[10] = '0';
  jpegBuf[11] = ',';
  jpegBuf[12] = '0';
  jpegBuf[13] = '0';
  jpegBuf[14] = '0';
  jpegBuf[15] = ',';
  temp = jpegId;
  if(temp > 9) {
    jpegBuf[5] = '0' + temp / 10;
  }
  jpegBuf[6] = '0' + temp % 10;
  temp = jpegCount;
  if(temp > 99) {
    jpegBuf[8] = '0' + temp / 100;
    temp %= 100;
  }
  if(temp > 9) {
    jpegBuf[9] = '0' + temp / 10;
  }
  jpegBuf[10] = '0' + temp % 10;
  jpegBufPoint = 16;
}

void writeJpeg(const unsigned char * buf, const unsigned int len)
{
  uint16_t temp = 0;

  if(! jpegBufPoint) {
    writeJpegMsgHead();
  }

  if((jpegBufPoint + len) < sizeof(jpegBuf)) {
    memcpy(jpegBuf + jpegBufPoint, buf, len);
    jpegBufPoint += len;
  } else {
    temp = sizeof(jpegBuf) - jpegBufPoint;
    memcpy(jpegBuf + jpegBufPoint, buf, temp);
    while((HAL_GetTick() - jpegSendTick) < JPEG_BUFFER_INTERVAL);
    if(! showFirstImage)
      showBuf(jpegBuf, sizeof(jpegBuf));
    esp.sendDataInTransMode(jpegBuf, sizeof(jpegBuf));
    jpegSendTick = HAL_GetTick();
    jpegCount ++;
    writeJpegMsgHead();
    memcpy(jpegBuf + jpegBufPoint, buf + temp, len - temp);
    jpegBufPoint += len - temp;
  }
  jpegDataTotal += len;
}

void writeJpegEnd()
{
  jpegCount ++;
  jpegBuf[8] = 'E';
  jpegBuf[9] = 'N';
  jpegBuf[10] = 'D';
  jpegBuf[12] = '0' + (jpegCount > 99 ? (jpegCount / 100) : 0);
  jpegBuf[13] = '0' + (jpegCount > 9 ? (jpegCount / 10 % 10) : 0);
  jpegBuf[14] = '0' + jpegCount % 10;
  jpegId ++;
  if(jpegId == 3)
    jpegId = 0;
  while((HAL_GetTick() - jpegSendTick) < JPEG_BUFFER_INTERVAL);
  if(! showFirstImage)
    showBuf(jpegBuf, jpegBufPoint);
  esp.sendDataInTransMode(jpegBuf, jpegBufPoint);
  jpegSendTick = HAL_GetTick();
  uart1Print.print("jpegDataTotal: ");
  uart1Print.println(jpegDataTotal);
  jpegBufPoint = 0;
  jpegCount = 0;
  jpegDataTotal = 0;
  showFirstImage = true;
  // while(1);
}

extern OV7725 ov;
extern uint16_t ovFrameBuffer[];
uint8_t ovFrameBuffer888[320 * 3 * 8] = {0};

void tje_write(void* context, void* data, int size)
{
  uint16_t temp = 0;


  if(! jpegBufPoint) {
    writeJpegMsgHead();
  }

  if((jpegBufPoint + size) < sizeof(jpegBuf)) {
    memcpy(jpegBuf + jpegBufPoint, data, size);
    jpegBufPoint += size;
  } else {
    temp = sizeof(jpegBuf) - jpegBufPoint;
    memcpy(jpegBuf + jpegBufPoint, data, temp);
    while((HAL_GetTick() - jpegSendTick) < JPEG_BUFFER_INTERVAL);
    if(! showFirstImage)
      showBuf(jpegBuf, sizeof(jpegBuf));
    esp.sendDataInTransMode(jpegBuf, sizeof(jpegBuf));
    jpegSendTick = HAL_GetTick();
    jpegCount ++;
    writeJpegMsgHead();
    memcpy(jpegBuf + jpegBufPoint, (uint8_t*) ((uint8_t*) data + temp), size - temp);
    jpegBufPoint += size - temp;
  }
  jpegDataTotal += size;
}

void tje_get_next_line(unsigned char *src_data, int size)
{
  ov.getFrameBuffer(8, ovFrameBuffer);
  for(uint16_t i = 0; i < (320 * 8); i ++) {
    src_data[0] = (ovFrameBuffer[i] & 0xf800) >> 8;
    src_data[1] = (ovFrameBuffer[i] & 0x07e0) >> 3;
    src_data[2] = (ovFrameBuffer[i] & 0x001f) << 3;
    src_data += 3;
  }
}

bool EspConnectServerSteps()
{
  static uint16_t step;
  static uint32_t lastTick = 0;

  switch(step) {
    case 0: esp.closeCipserver(); step ++; break;
    case 1: esp.setCipmux(false); step ++; break;
    case 2: esp.setCipmode(true); step ++; break;
    case 3: {
      taConsole.println("station ip valid, try to connect server");
      taConsole.show();
      esp.connectUdpServer(serverIp, serverPort);
      step ++;
    } break;
    case 4: {
      if(esp.sta == EspAt::sta_lastCmdError) {
        step = 0;
      } else if(esp.sta == EspAt::sta_ready) {
        taConsole.println("connect to server ok");
        taConsole.show();
        step ++;
      }
    } break;
    case 5: {
      esp.enterTransMode();
      lastTick = HAL_GetTick();
      step ++;
    } break;
    case 6: {
      if(! esp.serverOk) {
        taConsole.println("server disconnected, reconnect");
        taConsole.show();
        esp.exitTransMode();
        step = 0;
      } else {
        if((HAL_GetTick() - lastTick) > 100) {
          uint16_t lineCount = 0;
          jpegCount = 0;
          lastTick = HAL_GetTick();
          // uart1Print.println("send data test");

          #if 0
          huffman_start(IMG_HEIGHT & -8, IMG_WIDTH & -8, writeJpeg);
          huffman_resetdc();
          ov.trigSingleFrame();
          ov.getFrameBufferStart(320, 240);
          while(ov.getFrameBufferAvailable()) {
            ov.getFrameBuffer(8, ovFrameBuffer);
            encode_line_yuv((uint8_t*) ovFrameBuffer, lineCount);
            lineCount ++;
          }
          huffman_stop();
          #endif

          #if 1
          ov.trigSingleFrame();
          ov.getFrameBufferStart(320, 240);
          tje_encode_with_func(tje_write, tje_get_next_line, 0, 1, 320, 240, 3, ovFrameBuffer888);
          #endif

          writeJpegEnd();
          // esp.sendDataInTransMode((uint8_t*) espTestData, sizeof(espTestData));
        }
      }
    } break;
  }
  return false;
}

void EspTask(void const * argument)
{
begin:
  bool espInitStepsOK = false, espCreateServerStepOK = false;

  esp.fCipServeReceive = EspCipReceive;
  esp.begin();
  taConsole.println("esp reset ...");
  taConsole.show();
  esp.hardReset();
  vTaskDelay(20);
  esp.hardPowerOn();
  vTaskDelay(6000);
  esp.clearReciveBuf();
  taConsole.println("esp reset done");
  taConsole.show();

  while((! esp.allReady()) && (esp.sta != EspAt::sta_timeout)) {
    vTaskDelay(10);
    esp.run();
  }
  if(! esp.deviceOk) {
    taConsole.println("esp init faild, timeout");
    taConsole.show();
  } else {
    taConsole.println("esp init success");
    taConsole.show();
    if(0) {
      esp.sendCmd("AT+CIUPDATE");
      for(;;) {
        vTaskDelay(20);
        esp.run();
      }
    }

    for(;;) {
      vTaskDelay(10);
      esp.run();
      if(esp.sta == EspAt::sta_timeout) {
        taConsole.println("esp operation timeout, reset esp");
        taConsole.show();
        // goto begin;
        break;
      } else {
        if(esp.sta == EspAt::sta_ready) {
          if(! espInitStepsOK) {
            espInitStepsOK = EspInitSteps();
          } else if(esp.haveStaIp) {
            EspConnectServerSteps();
          } else {
            espCreateServerStepOK = EspCreateServerSteps();
          }
        } else if(esp.sta == EspAt::sta_lastCmdError) {
          vTaskDelay(10);
        }
      }
    }

  }
  osThreadTerminate(espAppId);
}

void EspAppInit()
{
  HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);

  osThreadDef(espTask, EspTask, osPriorityNormal, 0, 512);
  espAppId = osThreadCreate(osThread(espTask), NULL);
  taskENABLE_INTERRUPTS();

  esp.printInfo();
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  esp.receiveOne(USART1->DR);
}
