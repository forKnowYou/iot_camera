#include "espAt.h"

// cmds register ----------------------------------------------------------------

#define FUN_CMD_DECLARE(fun)    EspAt::sta_t (fun) (const char *rec, EspAt *esp)

FUN_CMD_DECLARE(CmdAt);
FUN_CMD_DECLARE(CmdUartCur);
FUN_CMD_DECLARE(CmdCwmodeCur);
FUN_CMD_DECLARE(CmdCwsapCur);
FUN_CMD_DECLARE(CmdCifsr);
FUN_CMD_DECLARE(CmdCipmux);
FUN_CMD_DECLARE(CmdCipserver);
FUN_CMD_DECLARE(CmdCwjapDef);
FUN_CMD_DECLARE(CmdCwjapQuiry);
FUN_CMD_DECLARE(CmdCipStart);
FUN_CMD_DECLARE(CmdCipmode);
FUN_CMD_DECLARE(CmdCipClose);

const uint16_t espClientCmdsCountStart = __LINE__;
const EspAt::cmd_t \
cmd_at = {"AT", CmdAt, 0, 20},
cmd_uart_cur = {"UART_CUR", CmdUartCur, 0, 20},
cmd_cwmode_cur = {"CWMODE_CUR", CmdCwmodeCur, 0, 40},
cmd_cwsap_cur = {"CWSAP_CUR", CmdCwsapCur, 0, 2000},
cmd_cifsr = {"CIFSR", CmdCifsr, 0, 20},
cmd_cipmux = {"CIPMUX", CmdCipmux, 0, 20},
cmd_cipserver = {"CIPSERVER", CmdCipserver, 0, 20},
cmd_cwjap_def = {"CWJAP_DEF", CmdCwjapDef, CmdCwjapQuiry, 1000},
cmd_cipstart = {"CIPSTART", CmdCipStart, 0, 2000},
cmd_cipmode = {"CIPMODE", CmdCipmode, 0, 40},
cmd_cipclose = {"CIPCLOSE", CmdCipClose, 0, 1000};
const uint16_t espClientCmdsCountEnd = __LINE__;

const uint16_t espClientCmdsRegistedStart = __LINE__;
const EspAt::cmd_t *clientCmds[] = {
  &cmd_at,
  &cmd_uart_cur,
  &cmd_cwmode_cur,
  &cmd_cwsap_cur,
  &cmd_cifsr,
  &cmd_cipmux,
  &cmd_cipserver,
  &cmd_cwjap_def,
  &cmd_cipstart,
  &cmd_cipmode,
  &cmd_cipclose
};
const uint16_t espClientCmdsRegistedEnd = __LINE__;

FUN_CMD_DECLARE(CmdCipserverReceive);
FUN_CMD_DECLARE(CmdServerDisconnected);

const uint16_t espServerCmdsCountStart = __LINE__;
const EspAt::cmd_t \
cmd_cipserver_receive = {"+IPD,", CmdCipserverReceive, 0, 0},
cmd_serverDisconnected = {"CLOSED", CmdServerDisconnected, 0, 0};
const uint16_t espServerCmdsCountEnd = __LINE__;

const uint16_t espServerCmdsRegistedStart = __LINE__;
const EspAt::cmd_t *serviceCmds[] = {
  &cmd_cipserver_receive,
  &cmd_serverDisconnected
};
const uint16_t espServerCmdsRegistedEnd = __LINE__;

const char cmdReplyOK[] = "\r\nOK\r\n";
const char cmdReplyErr[] = "\r\nERROR\r\n";

// client cmds funs ----------------------------------------------------------------

EspAt::sta_t CmdAt(const char *rec, EspAt *esp)
{
  const char *p = strstr(rec, cmdReplyOK);
  if(p) {
    esp->deviceOk = true;
    return EspAt::sta_ready;
  }
  return EspAt::sta_lastCmdError;
}

EspAt::sta_t CmdUartCur(const char *rec, EspAt *esp)
{
  const char *p = strstr(rec, ",8,1,0,0\r\n\r\nOK\r\n");
  if(p) {
    esp->_uart.begin(esp->getBaud());
    LL_USART_EnableIT_RXNE(esp->_uart._uartx);
    esp->baudConfigured = true;
    esp->deviceOk = false;
    return EspAt::sta_ready;
  }
  return EspAt::sta_lastCmdError;
}

EspAt::sta_t CmdCwmodeCur(const char *rec, EspAt *esp)
{
  const char *p = strstr(rec, cmdReplyOK);
  if(p) {
    return EspAt::sta_ready;
  }
  return EspAt::sta_lastCmdError;
}

EspAt::sta_t CmdCwsapCur(const char *rec, EspAt *esp)
{
  const char *p = strstr(rec, cmdReplyOK);
  if(p) {
    esp->softApEstablished = true;
    return EspAt::sta_ready;
  }
  return EspAt::sta_lastCmdError;
}

EspAt::sta_t CmdCifsr(const char *rec, EspAt *esp)
{
  const char *p = strstr(rec, cmdReplyOK);
  const char *pt;
  if(p) {
    p = strstr(rec, "+CIFSR:APIP,\"");
    if(p) {
      pt = strstr(p, "\"\r\n");
      if(pt) {
        esp->haveApIp = true;
        memcpy(esp->apIp, p + sizeof("+CIFSR:APIP,\"") - 1, pt - p - sizeof("+CIFSR:APIP,\"") + 1);
      }
    }
    p = strstr(rec, "+CIFSR:STAIP,\"");
    if(p) {
      pt = strstr(p, "0.0.0.0");
      if(pt) {
        esp->haveStaIp = false;
      } else {
        pt = strstr(p, "\"\r\n");
        if(pt) {
          esp->haveStaIp = true;
          memcpy(esp->staIp, p + sizeof("+CIFSR:STAIP,\"") - 1, pt - p - sizeof("+CIFSR:STAIP,\"") + 1);
        }
      }
    }
    return EspAt::sta_ready;
  }
  return EspAt::sta_lastCmdError;
}

EspAt::sta_t CmdCipmux(const char *rec, EspAt *esp)
{
  const char *p = strstr(rec, cmdReplyOK);
  if(p) {
    return EspAt::sta_ready;
  }
  return EspAt::sta_lastCmdError;
}

EspAt::sta_t CmdCipserver(const char *rec, EspAt *esp)
{
  const char *p = strstr(rec, cmdReplyOK);
  if(p) {
    return EspAt::sta_ready;
  } else if(strstr(rec, "0")) {
    return EspAt::sta_ready;
  }
  return EspAt::sta_lastCmdError;
}

EspAt::sta_t CmdCwjapDef(const char *rec, EspAt *esp)
{
  const char *p = strstr(rec, cmdReplyOK);
  if(p) {
    return EspAt::sta_ready;
  }
  return EspAt::sta_lastCmdError;
}

EspAt::sta_t CmdCwjapQuiry(const char *rec, EspAt *esp)
{
  return EspAt::sta_lastCmdError;
}

EspAt::sta_t CmdCipStart(const char *rec, EspAt *esp)
{
  const char *p = strstr(rec, cmdReplyOK);
  if(p) {
    esp->serverOk = true;
    return EspAt::sta_ready;
  }
  esp->serverOk = false;
  return EspAt::sta_lastCmdError;
}

EspAt::sta_t CmdCipmode(const char *rec, EspAt *esp)
{
  const char *p = strstr(rec, cmdReplyOK);
  if(p) {
    return EspAt::sta_ready;
  }
  return EspAt::sta_lastCmdError;
}

EspAt::sta_t CmdCipClose(const char *rec, EspAt *esp)
{
  return EspAt::sta_ready;
}

// server cmds funs ----------------------------------------------------------------

EspAt::sta_t CmdCipserverReceive(const char *rec, EspAt *esp)
{
  const char *p = rec + strlen(cmd_cipserver_receive.cmd) + 2;
  uint16_t len = 0;
  while(*p != ':') {
    len = len * 10 + *p - '0';
    p ++;
  }
  esp->fCipServeReceive(p + 1, len);
  return EspAt::sta_ready;
}

EspAt::sta_t CmdServerDisconnected(const char *rec, EspAt *esp)
{
  esp->serverOk = false;
  return EspAt::sta_ready;
}

// EspAt public ----------------------------------------------------------------

EspAt::EspAt(UartPrint uart, char *recBuf, uint16_t recBufLen, uint32_t baud, funGetTick fGetTick, Pin rst) :
             _uart(uart), _recBuf(recBuf), _recBufLen(recBufLen), _baud(baud), _fGetTick(fGetTick), _rst(rst)
{
  _recBufPoint = 0;
  _lastRecTick = 0;
  lastCmdTick = 0;
  _temp = 0xffff;
  _operationTimeOut = 8000;
  _analyzeWaitTime = 20;

  deviceOk = false;
  receiveAnalyzed = false;
  sta = sta_ready;
  baudConfigured = false;
  wifiMode = wifiMode_unkown;
  haveApIp = false;
  haveStaIp = false;
}

void EspAt::begin()
{
  _rst.setOut(Pin::high, Pin::pp);
  hardReset();
  memset(apIp, 0, sizeof(apIp));
  memset(staIp, 0, sizeof(apIp));
  memset(_recBuf, 0, _recBufLen);
}

bool EspAt::allReady()
{
  return deviceOk && baudConfigured && (sta != sta_timeout);
}

void EspAt::printInfo()
{
  uart1Print.println("espAt info:");
  uart1Print.print("client cmds count:");
  uart1Print.println(espClientCmdsCountEnd - espClientCmdsCountStart - 2);
  uart1Print.print("client cmds registed count:");
  uart1Print.println(espClientCmdsRegistedEnd - espClientCmdsRegistedStart - 3);
  uart1Print.print("server cmds count:");
  uart1Print.println(espServerCmdsCountEnd - espServerCmdsCountStart - 2);
  uart1Print.print("server cmds registed count:");
  uart1Print.println(espServerCmdsRegistedEnd - espServerCmdsRegistedStart - 3);
}

void EspAt::hardReset()
{
  _rst.reset();
  clearReciveBuf();
  memset(apIp, 0, sizeof(apIp));
  memset(staIp, 0, sizeof(staIp));

  _recBufPoint = 0;
  _lastRecTick = 0;
  lastCmdTick = 0;
  _temp = 0xffff;
  _analyzeWaitTime = 20;

  deviceOk = false;
  sta = sta_ready;
  receiveAnalyzed = false;
  baudConfigured = false;
  wifiMode = wifiMode_unkown;
  haveApIp = false;
  haveStaIp = false;
}

void EspAt::hardPowerOn()
{
  _rst.set();
}

void EspAt::setWifiMode(wifiMode_t mode)
{
  if(canISendCmd()) {
    sendCmdWithParaHead(cmd_cwmode_cur.cmd);
    _uart.println(mode);
    wifiMode = mode;
    sendCmdEnd(cmd_cwmode_cur.waitTime);
  }
}

void EspAt::setSoftAp(const char *name, const char *pwd)
{
  if(canISendCmd()) {
    sendCmdWithParaHead(cmd_cwsap_cur.cmd);
    _uart.print("\"");
    _uart.print(name);
    _uart.print("\"");
    _uart.print(",\"");
    _uart.print(pwd);
    _uart.println("\",5,3");
    sendCmdEnd(cmd_cwsap_cur.waitTime);
  }
}

void EspAt::getSoftApIp()
{
  if(canISendCmd()) {
    _uart.print("AT+");
    _uart.println(cmd_cifsr.cmd);
    sendCmdEnd(cmd_cifsr.waitTime);
  }
}

void EspAt::setCipmux(bool on)
{
  if(canISendCmd()) {
    sendCmdWithParaHead(cmd_cipmux.cmd);
    _uart.println(on ? "1" : "0");
    sendCmdEnd(cmd_cipmux.waitTime);
  }
}

void EspAt::setCipserve(bool on, uint16_t port)
{
  if(canISendCmd()) {
    sendCmdWithParaHead(cmd_cipserver.cmd);
    _uart.print(on ? "1" : 0);
    _uart.print(",");
    _uart.println(port);
    sendCmdEnd(cmd_cipserver.waitTime);
  }
}

void EspAt::closeCipserver()
{
  if(canISendCmd()) {
    sendCmdWithParaHead(cmd_cipserver.cmd);
    _uart.println(0);
    sendCmdEnd(cmd_cipserver.waitTime);
  }
}

void EspAt::setStation(const char *name, const char *pwd)
{
  if(canISendCmd()) {
    sendCmdWithParaHead(cmd_cwjap_def.cmd);
    _uart.print("\"");
    _uart.print(name);
    _uart.print("\",\"");
    _uart.print(pwd);
    _uart.println("\"");
    setAnalyzeTimeOut(2000);
    sendCmdEnd(cmd_cwjap_def.waitTime);
  }
}

void EspAt::getStationIp()
{
  if(canISendCmd()) {
    sendCmdQuiry(cmd_cwjap_def.cmd);
    sendCmdEnd(cmd_cwjap_def.waitTime);
  }
}

void EspAt::connectTcpServer(const char *ip, uint16_t port)
{
  if(canISendCmd()) {
    sendCmdWithParaHead(cmd_cipstart.cmd);
    _uart.print("\"TCP\",\"");
    _uart.print(ip);
    _uart.print("\",");
    _uart.println(port);
    sendCmdEnd(cmd_cipstart.waitTime);
  }
}

void EspAt::connectUdpServer(const char *ip, uint16_t port)
{
  if(canISendCmd()) {
    sendCmdWithParaHead(cmd_cipstart.cmd);
    _uart.print("\"UDP\",\"");
    _uart.print(ip);
    _uart.print("\",");
    _uart.print(port);
    _uart.print(",");
    _uart.print(1100);
    _uart.print(",");
    _uart.println(0);  // enable transparent mode
    sendCmdEnd(cmd_cipstart.waitTime);
  }
}

void EspAt::enterTransMode()
{
  if(canISendCmd()) {
    sendCmd("AT+CIPSEND");
  }
}

void EspAt::sendDataInTransMode(uint8_t *dat, uint32_t len)
{
  _uart.printBuf(dat, len);
}

void EspAt::exitTransMode()
{
  if(canISendCmd()) {
    sendCmd("+++");
  }
}

void EspAt::sendDataToServer(uint8_t *dat, uint32_t len)
{
  if(canISendCmd() && serverOk) {
    sendCmdWithParaHead("CIPSEND");
    _uart.println(len);
    sendCmdEnd(2);
    while((_fGetTick() - lastCmdTick) < 2);
    if(! receiveAnalyzed) {
      if(strstr((const char *) _recBuf, "\r\n>")) {
        while(len --) {
          _uart.printOne(*dat);
          dat ++;
        }
      }
      while((_fGetTick() - lastCmdTick) < 200);
      uart1Print.println("\r\n-------------------------------- esp received start --------------------------------");
      uart1Print.print((const char*) _recBuf);
      uart1Print.println("\r\n-------------------------------- esp received end --------------------------------");
      clearReciveBuf();
      clearSta();
    }
  }
}

void EspAt::setCipmode(bool on)
{
  if(canISendCmd()) {
    sendCmdWithParaHead(cmd_cipmode.cmd);
    _uart.println(on ? "1" : "0");
    sendCmdEnd(cmd_cipmode.waitTime);
  }
}

void EspAt::clearSta()
{
  sta = sta_ready;
}

const uint32_t bauds[] = {9600, 14400, 19200, 38400, 56000, 57600, 115200, 1280000, 230400, 256000, 460800, 500000, 512000, 600000, 750000, 921600, 1000000, 1500000, 2000000};

void EspAt::run()
{
  uint32_t tick = _fGetTick();
  if((! receiveAnalyzed) && ((tick - _lastRecTick) > _analyzeWaitTime)) {
    uart1Print.println("\r\n-------------------------------- esp received start --------------------------------");
    uart1Print.print((const char*) _recBuf);
    uart1Print.println("\r\n-------------------------------- esp received end --------------------------------");
    for(uint16_t i = 0; i < (sizeof(clientCmds) / sizeof(clientCmds[0])); i ++) {
      const char *p = strstr((const char*) _recBuf, clientCmds[i]->cmd);
      if(p) {
        if(*(p + strlen(clientCmds[i]->cmd)) == '?') {
          if(clientCmds[i]->fAnalyzeQuiry) {
            sta = clientCmds[i]->fAnalyzeQuiry(p, this);
          }
        } else {
          sta = clientCmds[i]->fAnalyzeSet(p, this);
        }
      }
    }
    for(uint16_t i = 0; i < (sizeof(serviceCmds) / sizeof(serviceCmds[0])); i ++) {
      const char *p = strstr((const char*) _recBuf, serviceCmds[i]->cmd);
      if(p) {
        serviceCmds[i]->fAnalyzeSet(p, this);
      }
    }
    setAnalyzeTimeOut(20);
    clearReciveBuf();
  }

  if((sta == sta_busy) && ((tick - lastCmdTick) > _operationTimeOut))
    sta = sta_timeout;

  if(! deviceOk) {
    if((_temp == 0xffff) || baudConfigured) {
      _temp = 0;
      setBaud(_baud);
    } else {
      if(tick - lastCmdTick > _analyzeWaitTime) {
        if(_temp < (sizeof(bauds) / sizeof(bauds[0]))) {
          sta = sta_ready;
          uart1Print.print("now test baud:");
          uart1Print.println(bauds[_temp]);
          setBaud(bauds[_temp]);
          _temp ++;
        }
      }
    }
    sendCmd(cmd_at.cmd);
  } else {
    if(! baudConfigured)
      setBaudCmd(_baud);
  }
}

void EspAt::sendCmd(const char *cmd)
{
  if(sta == sta_ready) {
    _uart.println(cmd);
    sendCmdEnd(20);
  }
}

void EspAt::receiveOne(uint8_t dat)
{
  if(_recBufPoint < (_recBufLen - 1)) {
    _recBuf[_recBufPoint] = dat;
    _recBufPoint ++;
  }
  receiveAnalyzed = false;
  _lastRecTick = _fGetTick();
}

void EspAt::clearReciveBuf()
{
  memset(_recBuf, 0, _recBufPoint);
  _recBufPoint = 0;
  receiveAnalyzed = true;
}

bool EspAt::canISendCmd()
{
  return deviceOk && (sta == sta_ready);
}

// EspAt private ----------------------------------------------------------------

void EspAt::sendCmdWithParaHead(const char *cmd)
{
  _uart.print("AT+");
  _uart.print(cmd);
  _uart.print("=");
}

void EspAt::sendCmdQuiry(const char *cmd)
{
  _uart.print("AT+");
  _uart.print(cmd);
  _uart.println("?");
}

void EspAt::sendCmdEnd(uint16_t waitTime)
{
  lastCmdTick = _fGetTick();
  sta = sta_busy;
  setAnalyzeTimeOut(waitTime);
}

void EspAt::setBaud(uint32_t baud)
{
  _uart.begin(baud);
  LL_USART_EnableIT_RXNE(_uart._uartx);
}

void EspAt::setBaudCmd(uint32_t baud)
{
  if(canISendCmd()) {
    sendCmdWithParaHead(cmd_uart_cur.cmd);
    _uart.print(baud);
    _uart.println(",8,1,0,0");
    sendCmdEnd(20);
  }
}
