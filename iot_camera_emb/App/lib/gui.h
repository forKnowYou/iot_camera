#ifndef GUI_H
#define GUI_H

#include "stdint.h"
#include "string.h"

#include "print.h"

typedef uint16_t color_t;

typedef void (*FunFillRect)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, color_t color);

class Color {
public:
  enum {
    black = 0x0000,
    red = 0xf800,
    green = 0x07e0,
    blue = 0x001f,
    white = 0xffff,
    navy = 0x000f,
    darkGreen = 0x03e0,
    darkCyan = 0x03ef,
    maroon = 0x7800,
    purple = 0x780f,
    olive = 0x7be0,
    lightGray = 0xc618,
    darkGray = 0x7bef,
    cyan = 0x07ff,
    magenta = 0xf81f,
    yellow = 0xffe0
  };
};

class Widget {
public:
  Widget(const char * name, uint16_t x, uint16_t y, uint16_t levelIndex, uint16_t borderWidth = 1, color_t borderColor = Color::white, color_t background = Color::white);

  virtual void flushGraphic(FunFillRect fun);
  virtual void flush(FunFillRect fun);

  const char *_name;
  uint16_t _x, _y;
  uint16_t _levelIndex;
  uint16_t _borderWidth;

  color_t _borderColor, _background;

  bool _visible;
};

class WidgetWithText {
public:
  WidgetWithText(uint8_t *textBuffer, uint16_t vInterval = 1, uint16_t hInterval = 1, color_t textColor = Color::black);

  virtual void flushText(FunFillRect fun, uint16_t x, uint16_t y);
  uint16_t getTextPixelLen();

  uint8_t *_textBuffer;
  uint16_t _vInterval, _hInterval;
  color_t _textColor;
};

class Rect : public Widget {
public:
  Rect(const char * name, uint16_t x, uint16_t y, uint16_t w, uint16_t h,
       uint16_t levelIndex);

  virtual void flushGraphic(FunFillRect fun);

  uint16_t _w, _h;
};

class RoundRect : public Widget {
public:
  RoundRect(const char * name, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r,
            uint16_t levelIndex);

  virtual void flushGraphic(FunFillRect fun);

  uint16_t _w, _h, _r;
  bool _gradient;
};

class TextArea : public Rect, public WidgetWithText, public Print {
public:
  TextArea(const char * name, uint16_t x, uint16_t y, uint16_t w, uint16_t h,
           uint16_t levelIndex, uint8_t *textBuffer, uint16_t len);

  virtual void begin(uint32_t baud);
  virtual void flush(FunFillRect fun);
  void show();

  const uint16_t _len;

private:
  virtual void printOne(uint8_t ch);

  FunFillRect _funFillRect;

  uint16_t _cursorX, _cursorY;
  uint16_t _textBufferUsed;
};

class Button : public RoundRect, public WidgetWithText {
public:
  Button(const char * name, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r,
         uint16_t levelIndex, const char * text);

  virtual void flush(FunFillRect fun);
};

class Page {
public:
  Page(const char *name, Widget **widgets, uint16_t widgetsLen, color_t background);

  void fresh(FunFillRect fun);
  Widget *getWidgetByName(const char * name);

  const char *_name;
  Widget **_widgets;
  uint16_t _widgetsLen;
  color_t _background;

  uint16_t _id;
};

class GUI {
public:
  GUI();

  void begin(FunFillRect funFillRect, uint16_t width, uint16_t height, Page *pages, uint16_t len);
  void showPage(const char *pageName);

  uint16_t _width, _height;
  uint16_t _currentPageId;

private:
  FunFillRect _funFillRect;
  Page *_pages;
  uint16_t _pageLen;
};

extern TextArea taConsole;

#endif
