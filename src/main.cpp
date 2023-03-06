#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define Dht11 A0
#define RAIN A1
#define LDR A2
#define keybord A3

//U8G2_KS0108_128X64_F u8g2(U8G2_R2, 4, 5, 6, 7, 8, 9, 10, 11, 3, 2, 12, 13, U8X8_PIN_NONE, U8X8_PIN_NONE);
U8G2_KS0108_128X64_F u8g2(U8G2_R2, 11, 10, 9, 8, 7, 6, 5, 4, 12, 13, 3, 2, U8X8_PIN_NONE, U8X8_PIN_NONE);
//U8G2_KS0108_128X64_F u8g2(rotation, d0, d1, d2, d3, d4, d5, d6, d7, E, RS, cs1, cs2, U8X8_PIN_NONE ,U8X8_PIN_NONE);
/////////////////////////////////////////////////////////////////////////////////////////
#include <SimpleDHT.h>
SimpleDHT11 dht11(Dht11);

byte H = 0;
byte T = 0;

int err = SimpleDHTErrSuccess;

float val_float = 0;
int val_int = 0;
int val_fra = 0;
/////////////////////////////////////////////////////////////////////////////////////////
#include "RTClib.h"
RTC_DS1307 rtc;

int current = 0;
int previous = 0;

int s = 0;
//int m = 0;
int h = 0;
int ls = 0;
int lm = 0;
int lh = 0;
int corser = 0;

int a = 0;
/////////////////////////////////////////////////////////////////////////////////////////

int k = 0;
uint8_t m = 24;
bool setmode = false;

void setup() {
  Serial.begin(9600);

  u8g2.begin();
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);

  if (! rtc.begin()) {
    Serial.print("RTC not working");
    delay(10000);
  }
  if (! rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  DateTime now = rtc.now();

  k = Readkey(keybord, 5);
  if (k >= 3) {
    setmode = true;
    s = now.second();
    m = now.minute();
    h = now.hour();
    corser = 0;
  } else {
    setmode = false;
  }
  while (Readkey(keybord, 5) != 0) {}
}

void loop() {
  if (setmode == true) {
    u8g2.clearBuffer();
    u8g2.setDrawColor(1);
    u8g2.setFont(u8g2_font_t0_22b_tf);
    k = Readkey(keybord, 5);
    if (k == 1) {
      if (corser < 2) {
        corser = corser + 1;
      } else {
        rtc.adjust(DateTime(2022, 1, 1, h, m, s));
        setmode = false;
      }
      /////////////////////////////////////////////////////////////////////////////////////////
    } else if (k == 2) {
      if (corser == 0) {
        if (h < 24) {
          h = h + 1;
        } else {
          h = 0;
        }
      } else if (corser == 1) {
        if (m < 59) {
          m = m + 1;
        } else {
          m = 0;
        }
      } else if (corser == 2) {
        if (s < 59) {
          s = s + 1;
        } else {
          s = 0;
        }
      }
      /////////////////////////////////////////////////////////////////////////////////////////
    } else if (k == 3) {
      if (corser == 0) {
        if (h > 0) {
          h = h - 1;
        } else {
          h = 24;
        }
      } else if (corser == 1) {
        if (m > 0) {
          m = m - 1;
        } else {
          m = 59;
        }
      } else if (corser == 2) {
        if (s > 0) {
          s = s - 1;
        } else {
          s = 59;
        }
      }
      /////////////////////////////////////////////////////////////////////////////////////////
    } else if (k > 3) {
      if (corser > 0) {
        corser = corser - 1;
      } else {
        setmode = false;
        corser = 0;
      }
    }

    char buf3[5] = "00.0";
    strcpy(buf3, u8x8_u8toa(h, 2));
    u8g2.drawStr(0, 0, buf3);
    u8g2.drawStr(22, 0, ":");
    strcpy(buf3, u8x8_u8toa(m, 2));
    u8g2.drawStr(33, 0, buf3);
    u8g2.drawStr(55, 0, ":");
    strcpy(buf3, u8x8_u8toa(s, 2));
    u8g2.drawStr(66, 0, buf3);

    if (corser == 0) {
      u8g2.drawStr(0, 18, "--      ");
    } else if (corser == 1) {
      u8g2.drawStr(0, 18, "   --   ");
    } else if (corser == 2) {
      u8g2.drawStr(0, 18, "      --");
    }

    u8g2.sendBuffer();
    delay(500);
    //    while (Readkey(keybord, 5) != 0) {}

  } else {
    DateTime now = rtc.now();
    current = now.second();
    if (current != previous) {
      previous = current;

      u8g2.clearBuffer();
      u8g2.setDrawColor(1);
      u8g2.setFont(u8g2_font_profont12_tr);
      char buf1[9] = "hh:mm:ss";
      u8g2.drawStr(2, 5, now.toString(buf1));

      char buf3[5] = "00.0";
      u8g2.setDrawColor(1);

      u8g2.setFont(u8g2_font_t0_22b_tf);
      a = analogRead(RAIN);
      a = map(a, 0, 1023, 100,0);
      strcpy(buf3, u8x8_u8toa(a, 3));
      u8g2.drawStr(4, 24, buf3);
      u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
      if (a < 50) {
        u8g2.drawGlyph(44, 24, 64);
      } else {
        u8g2.drawGlyph(44, 24, 67);
      }

      u8g2.setFont(u8g2_font_t0_22b_tf);
      a = analogRead(LDR);
      a = map(a, 0, 1023, 100,0);
      strcpy(buf3, u8x8_u8toa(a, 3));
      u8g2.drawStr(4, 43, buf3);
      u8g2.setFont(u8g2_font_open_iconic_weather_2x_t);
      if (a < 30) {
        u8g2.drawGlyph(44, 43, 66);
      } else {
        u8g2.drawGlyph(44, 43, 69);
      }

      u8g2.setFont(u8g2_font_t0_22b_tf);
      val_int = (int)T;   // compute the integer part of the float
      val_float = (abs(T) - abs(val_int)) * 10;
      val_fra = (int)val_float;
      sprintf(buf3, "%d.%d", val_int, val_fra);
      u8g2.drawStr(66, 43, buf3);

      val_int = map(val_int, 0, 100, 45, 0);
      u8g2.drawRFrame(115, 4, 7, 48, 3);
      u8g2.drawRFrame(116, 4 + val_int, 5, 48 - val_int, 0);
      u8g2.drawRFrame(117, 4 + val_int, 2, 48 - val_int, 0);
      u8g2.drawDisc(118, 54, 6);

      u8g2.setFont(u8g2_font_t0_22b_tf);
      val_int = (int)H;   // compute the integer part of the float
      val_float = (abs(H) - abs(val_int)) * 10;
      val_fra = (int)val_float;
      sprintf(buf3, "%d%%", val_int);
      u8g2.drawStr(66, 10, buf3);

      u8g2.setFont(u8g2_font_open_iconic_thing_2x_t);
      u8g2.drawGlyph(97, 8, 72);

      u8g2.setDrawColor(1);
      u8g2.drawRFrame(0, 23, 64, 41, 5);
      u8g2.drawRFrame(63, 0, 65, 64, 5);

      u8g2.sendBuffer();
      GETTH();
    }
  }
}
void GETTH() {
  if ((err = dht11.read(&T, &H, NULL)) != SimpleDHTErrSuccess) {
    return;
  }
}

int Readkey(int pin, int keys) {
  int analog;
  analog = analogRead(pin);
  analog = map(analog, 0, 1023, 0, keys);
  return analog;
}