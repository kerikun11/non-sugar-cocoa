/**
 * @file scene_clock.hpp
 * @author Ryotaro Onuki (kerikun11@gmail.com)
 * @brief 現在時刻を表示するシーン
 * @version 0.1
 * @date 2018-11-30
 */
#pragma once

// コンパイルエラーを防ぐため， Arduino.h で定義されているマクロをundef
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#include <chrono>
#include <string>

#include "hardware/hardware.h"
#include "scene/event.hpp"
#include "scene/scene.hpp"
#include "scene/scene_alarming.hpp"
#include "scene/scene_configure_alarm.hpp"

namespace scene {

// 現在時刻を表示する Scene
class SceneClock : public Scene {
public:
  // コンストラクタ，必要なものがあれば受け取る仕様にする
  SceneClock(std::shared_ptr<hardware::Hardware> &m_hardware)
      : m_hardware(m_hardware) {}

  /// 定期的に (タイマーイベントごとに) 呼ばれる。
  virtual EventResult tick() override {
    // TODO: 時刻が変わったときだけ更新
    updateDisplayClock();
    return EventResultKind::Continue;
  }

  /// シーンがスタックのトップに来たとき呼ばれる。
  virtual EventResult activated() override {
    // ごみを消去
    updateDisplayClock(true); //< 完全再描画
    DrawAlarmTime(
        sugar::TimeOfDay()); //< 来たときのみアラーム時刻の描画処理をする
    log_i("SceneClock activated()");
    return EventResultKind::Continue;
  }

  /// ボタン
  virtual EventResult buttonAPressed() override {
    /// アラーム設定へ
    return EventResult(EventResultKind::PushScene,
                       static_cast<void *>(new SceneConfigureAlarm(
                           m_hardware->alarm().alarmTimeSetter())));
  }

  /// ボタン
  virtual EventResult buttonBPressed() override {
    /// アラーム設定へ
    return EventResult(EventResultKind::PushScene,
                       static_cast<void *>(new SceneAlarming(m_hardware)));
  }

protected:
  std::shared_ptr<hardware::Hardware> m_hardware;

  void updateDisplayClock(bool clean = false) {
    //アニメーションのための数値
    static uint8_t frame = 0;
    frame++;
    // 現在時刻の取得
    auto now = std::chrono::system_clock::now().time_since_epoch() +
               std::chrono::hours(9); //< 日本時間: +9h
    int hh = std::chrono::duration_cast<std::chrono::hours>(now).count() % 24;
    int mm = std::chrono::duration_cast<std::chrono::minutes>(now).count() % 60;
    int ss = std::chrono::duration_cast<std::chrono::seconds>(now).count() % 60;
    int ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(now).count() %
        1000;
    //時刻描画
    displayClock(clean, hh, mm, ss, ms);
    //ボタン説明の描画
    const int tMax = 10, height = 3, topY = 200;
    const int t = frame % tMax;
    int y; //アニメーションのためのy方向変位
    if (t < tMax / 2) {
      y = topY - t * (t - tMax / 2) / (tMax * tMax / 16 / height) + height;
    } else {
      y = topY + (t - tMax / 2) * (t - tMax) / (tMax * tMax / 16 / height) +
          height;
    }
    M5.Lcd.setTextColor(TFT_PINK, TFT_BLACK);
    M5.Lcd.fillRect(42, topY, 50, y - topY, TFT_BLACK); //文字を消す
    M5.Lcd.drawString(
        "SET", 42, y,
        4); //フォントサイズ4で3文字の描画を左ボタンの上にするなら、(42,205)に描画
  }

  void displayClock(bool clean, int hh, int mm, int ss, int ms) const {
    //直前に描画を行った分・秒の値。これが現在時刻と異なっていたら描画する。分・秒が絶対に取りえない値で初期化することで、初めてのdrawClock()では描画処理が必ず発生するようにする
    static byte omm = 99;
    static byte xcolon = 0, xsecs = 0;
    // 明るく描画するか
    static bool lightColon = true; // 500msごとに切り替える

    //文字色設定
    M5.Lcd.setTextColor(TFT_YELLOW, TFT_BLACK);
    //描画準備
    if (clean) {
      M5.Lcd.clear();
      M5.Lcd.drawCentreString("Alarm Clock", 160, 0, 8);
    }
    //描画位置
    int xpos = 0;
    int ypos = 85; // Top left corner ot clock text, about half way down
    int ysecs = ypos + 24;

    //描画処理
    //毎分の時間・分の描画（分が変更していれば、時間は変わっていなくても時間を描画しなおす）
    if (clean || omm != mm) {
      // ommの更新をし、分の値が変わらないうちは描画処理を行わないようにする
      omm = mm;
      //時間の描画、0でいいので2桁目を描画する
      if (hh < 10) {
        xpos += M5.Lcd.drawChar('0', xpos, ypos,
                                8); // Add hours leading zero for 24 hr clock
      }
      xpos += M5.Lcd.drawNumber(hh, xpos, ypos, 8); // Draw hours
      xcolon = xpos; //秒の描画において":"を描画するのに用いる
      //時間と分の間の":"の描画
      xpos +=
          M5.Lcd.drawChar(':', xpos, ypos - 8, 8); //どうせ暗く塗りつぶされる
      //分の描画、0でいいので2桁目を描画する
      if (mm < 10) {
        xpos += M5.Lcd.drawChar('0', xpos, ypos, 8); // Add minutes leading zero
      }
      xpos += M5.Lcd.drawNumber(mm, xpos, ypos, 8); // Draw minutes
      xsecs = xpos; // Sae seconds 'x' position for later display updates
    }
    //毎秒の秒の描画
    if (clean || lightColon != (ms < 500)) {
      // ossの更新をし、秒の値が変わっていないうちは描画処理を行わないようにする
      xpos = xsecs;
      lightColon = (ms < 500);
      //":"の描画
      if (!lightColon) {
        // lightColonを見て：の明るさの描画を決める
        M5.Lcd.setTextColor(0x39C4, TFT_BLACK); //文字色を暗くする
        M5.Lcd.drawChar(
            ':', xcolon, ypos - 8,
            8); //時間と分の間の":"を暗く描画（分の描画で黄色く描画してても暗い":"で上書きしてしまう）
        xpos += M5.Lcd.drawChar(':', xsecs, ysecs, 6); //分と秒の間の":"を描画
        M5.Lcd.setTextColor(TFT_YELLOW, TFT_BLACK); //文字色を黄色に戻す
      } else {
        M5.Lcd.drawChar(':', xcolon, ypos - 8,
                        8); //時間と分の間の":"を明るく描画
        xpos += M5.Lcd.drawChar(':', xsecs, ysecs, 6); //分と秒の間の":"を描画
      }
      //秒の描画、0でいいので2桁目を描画する
      if (ss < 10) {
        xpos += M5.Lcd.drawChar('0', xpos, ysecs, 6); // Add leading zero
      }
      M5.Lcd.drawNumber(ss, xpos, ysecs, 6); // Draw seconds
    }
  }

  // 画面上部にアラーム時刻を描画する関数
  void DrawAlarmTime(sugar::TimeOfDay tod) const {
    // らりお：todを文字列化して
    std::string str = "Alarm : 00:00:00";
    // strを描画
    M5.Lcd.drawString(str.c_str(), 10, 10, 2);
  }
};

}; // namespace scene
