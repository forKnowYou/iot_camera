#ifndef ESPAT_H
#define ESPAT_H

#include "print.h"
#include "string.h"
#include "pin.h"

class EspAt {
public:
  typedef uint32_t (*funGetTick)(void);

  typedef enum {
    sta_ready,
    sta_busy,
    sta_timeout,
    sta_lastCmdError
  } sta_t;

  typedef sta_t (*funAnalyze)(const char *rec, EspAt *esp);

  typedef struct {
    const char *cmd;
    funAnalyze fAnalyzeSet;
    funAnalyze fAnalyzeQuiry;
    uint16_t waitTime;
  } cmd_t;

  EspAt(UartPrint uart, char *recBuf, uint16_t recBufLen, uint32_t baud, funGetTick fGetTick, Pin rst);

  void begin();
  bool allReady();

  void printInfo();

  uint32_t getBaud() {return _baud;}
  void setOperationTimeOut(uint32_t t) { _operationTimeOut = t; }
  void setAnalyzeTimeOut(uint32_t t) { _analyzeWaitTime = t; }
  void hardReset();
  void hardPowerOn();

  typedef enum {
    wifiMode_unkown,
    wifiMode_station,
    wifiMode_softAp,
    wifiMode_staAndAp
  } wifiMode_t;

  void setWifiMode(wifiMode_t mode);
  void setSoftAp(const char *name, const char *pwd);
  void getSoftApIp();
  void setCipmux(bool on);
  void setCipserve(bool on, uint16_t port);
  void closeCipserver();
  void setStation(const char *name, const char *pwd);
  void getStationIp();
  void connectTcpServer(const char *ip, uint16_t port);
  void connectUdpServer(const char *ip, uint16_t port);
  void sendDataToServer(uint8_t *dat, uint32_t len);
  void enterTransMode();
  void sendDataInTransMode(uint8_t *dat, uint32_t len);
  void exitTransMode();
  void setCipmode(bool on);
  void clearSta();

  void run();

  typedef void (*funAnalyzeString)(const char *p, uint16_t len);

  void sendCmd(const char *cmd);
  void receiveOne(uint8_t dat);
  void clearReciveBuf();
  bool canISendCmd();

  UartPrint _uart;
  funAnalyzeString fCipServeReceive;

public:
  // status
  bool \
deviceOk,
receiveAnalyzed,
baudConfigured,
softApEstablished,
haveApIp,
haveStaIp,
serverOk
;
  sta_t sta;

  wifiMode_t wifiMode;

  char apIp[16], staIp[16];

  uint32_t lastCmdTick;

private:
  void sendCmdWithParaHead(const char *cmd);
  void sendCmdQuiry(const char *cmd);
  void sendCmdEnd(uint16_t waitTime);
  void setBaud(uint32_t baud);
  void setBaudCmd(uint32_t baud);

  char *_recBuf;
  uint16_t _recBufLen, _recBufPoint;
  uint32_t _baud;
  funGetTick _fGetTick;
  uint32_t _lastRecTick;
  uint32_t _operationTimeOut;
  uint32_t _temp;
  Pin _rst;
  uint32_t _analyzeWaitTime;
};

#endif
