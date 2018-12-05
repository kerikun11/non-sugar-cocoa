/**
 * @file scene_set_clock.cpp
 * @author Masashi Oyama (mabo168general@gmail.com)
 * @brief 時刻合わせ場面のクラス
 * @version 0.1
 * @date 2018-12-01
 */

#pragma once

#include "../hardware/alarm_manager.hpp"
#include "../hardware/hardware.h"
#include "../time_of_day.hpp"
#include "scene.hpp"

// コンパイルエラーを防ぐため， Arduino.h で定義されているマクロをundef
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#include <chrono>

namespace scene {

/// Scene を継承する
class SceneConfigureAlarm : public Scene {
private:
  /// カーソル位置。
  enum class Cursor : int {
    /// 時
    Hour = 0,
    /// 分
    Minute = 1,
    /// 秒
    Second = 2,
  };

  /// アラーム時刻。
  sugar::TimeOfDay m_alarmTime;
  /// カーソル位置。
  Cursor m_cursor;
  /// 時刻設定機構。
  hardware::AlarmTimeSetter m_alarmTimeSetter;

public:
  SceneConfigureAlarm(hardware::AlarmTimeSetter timeSetter)
      : m_alarmTime{}, m_cursor{Cursor::Hour}, m_alarmTimeSetter{timeSetter} {}

  /// シーンがスタックのトップに来たとき呼ばれる。
  virtual EventResult activated() override {
    log_i("SceneSetAlarm activated()");
    // LCDのクリア
    M5.Lcd.clear();
    // 現在時刻で初期化
    m_alarmTime =
        sugar::TimeOfDay::fromUtcToJst(std::chrono::system_clock::now());
    //その他の初期化
    m_cursor = Cursor::Hour;
    // ごみを消去
    M5.Lcd.clear();
    return EventResultKind::Continue;
  }

  /// 定期的に (タイマーイベントごとに) 呼ばれる。
  virtual EventResult tick() override {
    // TODO: 時刻が変わったときだけ更新
    updateDisplay();
    return EventResultKind::Continue;
  }

  virtual EventResult buttonAPressed() override {
    decrement();
    updateDisplay();
    return EventResultKind::Continue;
  }
  virtual EventResult buttonBPressed() override {
    // カーソルを進める
    moveCursorNext();
    // カーソルを次の桁に移動後に「時」の位置にあるなら、
    // カーソル位置が一周したということなので時刻設定を完了する
    if (m_cursor == Cursor::Hour) {
      // 設定した時間を送信する。
      m_alarmTimeSetter.setAlarmTime(m_alarmTime);
      return EventResultKind::Finish;
    }
    // 時間設定を続行するので画面を更新する
    updateDisplay();

    return EventResultKind::Continue;
  }
  virtual EventResult buttonCPressed() override {
    increment();
    updateDisplay();
    return EventResultKind::Continue;
  }

private:
  void moveCursorNext() {
    // 編集箇所を次に進める
    m_cursor = static_cast<Cursor>((static_cast<int>(m_cursor) + 1) % 3);
  }

  void increment() {
    // カーソル位置の桁をインクリメント
    switch (m_cursor) {
    case Cursor::Hour:
      m_alarmTime += std::chrono::hours(1);
      break;
    case Cursor::Minute:
      m_alarmTime += std::chrono::minutes(1);
      break;
    case Cursor::Second:
      m_alarmTime += std::chrono::seconds(1);
      break;
    }
  }
  void decrement() {
    //数値をデクリメント
    switch (m_cursor) {
    case Cursor::Hour:
      m_alarmTime -= std::chrono::hours(1);
      break;
    case Cursor::Minute:
      m_alarmTime -= std::chrono::minutes(1);
      break;
    case Cursor::Second:
      m_alarmTime -= std::chrono::seconds(1);
      break;
    }
  }

  // 画面を更新する
  void updateDisplay() const {
    // ボタン上のUIの描画
    const int32_t rectY = 212;
    const int32_t width = 20;
    const int32_t height = 6;
    const int32_t leftX = 56;
    const int32_t rightX = 245;
    // 一番左は"-"を描画（長方形の描画を用いる）
    M5.Lcd.setTextColor(TFT_PINK, TFT_BLACK);
    // M5.Lcd.drawChar('-',53,200,6);//<
    // これは正しく表示されるが、なんとなく見づらい
    M5.Lcd.fillRect(leftX, rectY, width, height, TFT_PINK);
    // 中央は"->"を右方向にアニメーションして描画
    M5.Lcd.setTextColor(TFT_PINK, TFT_BLACK);
    M5.Lcd.drawString("NEXT", 130, 205, 4);
    // 一番右は"+"を描画（長方形の描画を用いる）
    M5.Lcd.setTextColor(TFT_PINK, TFT_BLACK);
    // M5.Lcd.drawChar('+',250,200,6);//<
    // サイズ6だと+が表示されない、意味わからん……
    M5.Lcd.fillRect(rightX, rectY, width, height, TFT_PINK);
    M5.Lcd.fillRect(rightX + (width - height) / 2, rectY + (height - width) / 2,
                    height, width, TFT_PINK);

    // 時刻部分の更新
    updateDisplayClock();
  }

  // 画面のうち時刻部分を更新する
  void updateDisplayClock() const {
    //描画準備
    //文字色設定
    M5.Lcd.setTextColor(TFT_YELLOW, TFT_BLACK);
    //描画位置
    int xpos = 0;
    int ypos = 85; // Top left corner ot clock text, about half way down
    int ysecs = ypos + 24;

    //描画処理
    //時間の描画、0でいいので2桁目を描画する
    xpos += drawNumber2Char(m_alarmTime.hour(), xpos, ypos, 8);
    //時間と分の間の":"の描画
    xpos += M5.Lcd.drawChar(':', xpos, ypos - 8, 8);
    //分の描画、0でいいので2桁目を描画する
    xpos += drawNumber2Char(m_alarmTime.minute(), xpos, ypos, 8);
    //分と秒の間の":"の描画
    xpos += M5.Lcd.drawChar(':', xpos, ysecs, 6);
    //秒の描画、0でいいので2桁目を描画する
    xpos += drawNumber2Char(m_alarmTime.second(), xpos, ysecs, 6);
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
