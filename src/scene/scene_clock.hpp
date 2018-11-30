/**
 * @file scene_clock.hpp
 * @author Ryotaro Onuki (kerikun11@gmail.com)
 * @brief 現在時刻を表示するシーン
 * @version 0.1
 * @date 2018-11-30
 */
#pragma once

#include "hardware/hardware.h"
// #include "scene/scene.hpp"

namespace scene {

/// Scene を継承する
// class SceneClock : Scene {
class SceneClock {
public:
  SceneClock() {}

  void initialize() {
    // NTP Settings
    const char *ntpServer = "ntp.jst.mfeed.ad.jp"; //日本のNTPサーバー選択
    const long gmtOffset_sec = 9 * 3600; // 9時間の時差を入れる
    const int daylightOffset_sec = 0;    //夏時間はないのでゼロ
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  }
  void update() { void updateDisplayClock(); }

private:
  void updateDisplayClock() {
    static byte omm = 99, oss = 99;
    static byte xcolon = 0, xsecs = 0;
    static unsigned int colour = 0;

    uint8_t hh = 0, mm = 0, ss = 0;
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
      hh = timeinfo.tm_hour;
      mm = timeinfo.tm_min;
      ss = timeinfo.tm_sec;
    }
    M5.Lcd.setTextColor(TFT_YELLOW, TFT_BLACK);

    // Update digital time
    int xpos = 0;
    int ypos = 85; // Top left corner ot clock text, about half way down
    int ysecs = ypos + 24;

    if (omm != mm) { // Redraw hours and minutes time every minute
      omm = mm;
      // Draw hours and minutes
      if (hh < 10)
        xpos += M5.Lcd.drawChar('0', xpos, ypos,
                                8); // Add hours leading zero for 24 hr clock
      xpos += M5.Lcd.drawNumber(hh, xpos, ypos, 8); // Draw hours
      xcolon = xpos; // Save colon coord for later to flash on/off later
      xpos += M5.Lcd.drawChar(':', xpos, ypos - 8, 8);
      if (mm < 10)
        xpos += M5.Lcd.drawChar('0', xpos, ypos, 8); // Add minutes leading zero
      xpos += M5.Lcd.drawNumber(mm, xpos, ypos, 8);  // Draw minutes
      xsecs = xpos; // Sae seconds 'x' position for later display updates
    }
    if (oss != ss) { // Redraw seconds time every second
      oss = ss;
      xpos = xsecs;

      if (ss % 2) { // Flash the colons on/off
        M5.Lcd.setTextColor(0x39C4,
                            TFT_BLACK); // Set colour to grey to dim colon
        M5.Lcd.drawChar(':', xcolon, ypos - 8, 8);     // Hour:minute colon
        xpos += M5.Lcd.drawChar(':', xsecs, ysecs, 6); // Seconds colon
        M5.Lcd.setTextColor(TFT_YELLOW, TFT_BLACK); // Set colour back to yellow
      } else {
        M5.Lcd.drawChar(':', xcolon, ypos - 8, 8);     // Hour:minute colon
        xpos += M5.Lcd.drawChar(':', xsecs, ysecs, 6); // Seconds colon
      }

      // Draw seconds
      if (ss < 10)
        xpos += M5.Lcd.drawChar('0', xpos, ysecs, 6); // Add leading zero
      M5.Lcd.drawNumber(ss, xpos, ysecs, 6);          // Draw seconds
    }
  }
  // Function to extract numbers from compile time string
  static uint8_t conv2d(const char *p) {
    uint8_t v = 0;
    if ('0' <= *p && *p <= '9')
      v = *p - '0';
    return 10 * v + *++p - '0';
  }
};

}; // namespace scene