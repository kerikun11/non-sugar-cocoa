/**
 * @file scene_set_clock.cpp
 * @author Masashi Oyama (mabo168general@gmail.com)
 * @brief 時刻合わせ場面のクラス
 * @version 0.1
 * @date 2018-12-01
 */

#pragma once

#include "../hardware/hardware.h"
#include "scene.hpp"

namespace scene {

/// Scene を継承する
class SceneSetClock : public Scene {
public:
  SceneSetClock() {}

  /// シーンがスタックのトップに来たとき呼ばれる。
  virtual EventResult activated() override {
    log_i("SceneSetAlarm activated()");
    //実際には、すでに記録されている時刻通りに初期化する
    m_hour = 0;
    m_min = 0;
    m_sec = 0;
    //その他の初期化
    m_process = 0;
    // ごみを消去
    // M5.Lcd.fillScreen(0);
    return EventResultKind::Continue;
  }

  /// 定期的に (タイマーイベントごとに) 呼ばれる。
  virtual EventResult tick() override {
    // TODO: 時刻が変わったときだけ更新
    updateDisplayClock();
    return EventResultKind::Continue;
  }

  virtual EventResult buttonAPressed() override {
    proceedProcess();
    updateDisplayClock();
    return EventResultKind::Continue;
  }
  virtual EventResult buttonBPressed() override {
    increment();
    updateDisplayClock();
    return EventResultKind::Continue;
  }
  virtual EventResult buttonCPressed() override {
    // TODO: アラーム設定完了の処理
    return EventResultKind::Finish;
  }

private:
  uint8_t m_hour, m_min, m_sec; //時刻
  int m_process; //編集する場所。この書き方は汚い。(0:時間 1:分 2:秒)

  void proceedProcess() {
    //編集箇所を次に進める
    m_process = (m_process + 1) % 3;
    //ディスプレイの更新
    updateDisplayClock();
  }

  void increment() {
    //数値をインクリメント
    switch (m_process) {
    case (0):
      m_hour = (m_hour + 1) % 24;
      break;
    case (1):
      m_min = (m_min + 1) % 60;
      break;
    case (2):
      m_sec = (m_sec + 1) % 60;
      break;
    }
  }
  void decrement() {
    //数値をデクリメント
    switch (m_process) {
    case (0):
      m_hour = (m_hour + 23) % 24;
      break;
    case (1):
      m_min = (m_min + 59) % 60;
      break;
    case (2):
      m_sec = (m_sec + 59) % 60;
      break;
    }
  }
  void updateDisplayClock() const {
    M5.Lcd.print(m_hour);
    M5.Lcd.print(":");
    M5.Lcd.print(m_min);
    M5.Lcd.print(":");
    M5.Lcd.println(m_sec);

    //描画準備
    //文字色設定
    M5.Lcd.setTextColor(TFT_YELLOW, TFT_BLACK);
    //描画位置
    int xpos = 0;
    int ypos = 85; // Top left corner ot clock text, about half way down
    int ysecs = ypos + 24;

    //描画処理
    //時間の描画、0でいいので2桁目を描画する
    xpos += drawNumber2Char(m_hour, xpos, ypos, 8);
    //時間と分の間の":"の描画
    xpos += M5.Lcd.drawChar(':', xpos, ypos - 8, 8);
    //分の描画、0でいいので2桁目を描画する
    xpos += drawNumber2Char(m_min, xpos, ypos, 8);
    //分と秒の間の":"の描画
    xpos += M5.Lcd.drawChar(':', xpos, ysecs, 6);
    //秒の描画、0でいいので2桁目を描画する
    xpos += drawNumber2Char(m_sec, xpos, ysecs, 6);
  }
  int drawNumber2Char(uint8_t pal, int x, int y, int font) const {
    // 2文字で数字の描画(1桁は先頭に0をつけて描画する)
    int dx = 0; //文字列描画の横幅
    if (pal < 10) {
      dx += M5.Lcd.drawChar('0', x, y, font);
    }
    dx += M5.Lcd.drawNumber(pal, x + dx, y, font);
    return dx;
  }
};

}; // namespace scene
