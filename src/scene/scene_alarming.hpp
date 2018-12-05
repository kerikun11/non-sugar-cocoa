/**
 * @file scene_alarming.hpp
 * @author Sekihara Takeshi (seki.maq.kan@gmail.com)
 * @brief 現在時刻を表示するシーン
 * @version 0.1
 * @date 2018-12-02
 */
#pragma once

#include "hardware/hardware.h"
#include "scene/event.hpp"
#include "scene/scene.hpp"

// コンパイルエラーを防ぐため， Arduino.h で定義されているマクロをundef
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#include <chrono>

namespace scene {

// アラーム設定時刻になった後，アラーム音が鳴り続けている最中のScene
class SceneAlarming : public Scene {
public:
  // コンストラクタ，必要なものがあれば受け取る仕様にする
  SceneAlarming(std::shared_ptr<hardware::Hardware> &m_hardware)
      : m_hardware(m_hardware) {}

  /// シーンがスタックのトップに来たとき呼ばれる。
  virtual EventResult activated() override {
    log_i("SceneAlarming activated()");

    // LCDのクリア
    M5.Lcd.clear();
    //文字色設定
    M5.Lcd.setTextColor(TFT_YELLOW, TFT_BLACK);

    //アラーム音の再生を開始
    m_hardware->speaker().play(hardware::SpeakerManager::Music::Alarm);

    // hardware側の振動回数の初期化(Countを使用するのは一人だと仮定．よそで勝手に操作されると困る)
    m_hardware->shaking().resetCount();
    m_hardware->shaking().startCount();

    // 絶起ツイートの時間を設定
    m_timelimit_to_stop = std::chrono::system_clock::now() + alarming_duration;

    return EventResultKind::Continue;
  }

  /// 定期的に (タイマーイベントごとに) 呼ばれる。
  virtual EventResult tick() override {

    auto now_count = m_hardware->shaking().getCount();

    if (now_count >= max_count) {
      m_hardware->speaker().stop(); //音の停止

      //カウントの停止とリセット
      m_hardware->shaking().stopCount();
      m_hardware->shaking().resetCount();

      m_hardware->tweet().tweet(
          "はいプロ\n世界一起床が上手\n起床界のtourist\n布団時代の終"
          "焉を告げる者\n実質朝\n起床するために生まれてきた男");
      log_i("SceneAlarming kisyou_success Finish");
      return EventResultKind::Finish;
    }

    int remain_count = max_count - m_hardware->shaking().getCount();
    updateLcd(remain_count);

    //アラーム制限時間処理
    if (std::chrono::system_clock::now() > m_timelimit_to_stop) {

      //起床失敗 tweet & alarm停止
      m_hardware->speaker().stop();

      m_hardware->tweet().tweet("絶起");
      log_i("SceneAlarming kisyou_failed Finish");
      return EventResultKind::Finish;
    }

    return EventResultKind::Continue;
  }

  /// ボタンが押されても何もしないので，ボタン関連overrideはしない

protected:
  std::shared_ptr<hardware::Hardware> m_hardware;

  // アラームを止めるために振らなければならない回数
  const int max_count = 5;
  //アラームが鳴り続ける最大時間．これを過ぎるとぜっきTweet
  const std::chrono::seconds alarming_duration{10};

  std::chrono::system_clock::time_point m_timelimit_to_stop =
      std::chrono::system_clock::now();

private:
  void updateLcd(int remain_count) const {
    static int prev_count = 0;
    if (remain_count == prev_count)
      return;
    prev_count = remain_count;
    //描画位置
    int xpos = 160;
    int ypos = 85 - 24; // Top left corner ot clock text, about half way down

    //描画処理
    M5.Lcd.drawCentreString("Shake to Stop!", xpos, ypos, 4); // Draw hours
    ypos += 54;
    M5.Lcd.drawCentreString(String(remain_count, DEC), xpos, ypos, 8);
  }
};

}; // namespace scene
