/**
 * @file scene_alerming.hpp
 * @author Sekihara Takeshi (seki.maq.kan@gmail.com)
 * @brief 現在時刻を表示するシーン
 * @version 0.1
 * @date 2018-12-02
 */
#pragma once

#include "hardware/hardware.h"
#include "scene/event.hpp"
#include "scene/scene.hpp"
#include "scene_set_clock.hpp"

namespace scene {

// アラーム設定時刻になった後，アラーム音が鳴り続けている最中のScene
class SceneAlarming : public Scene {
public:
  // コンストラクタ，必要なものがあれば受け取る仕様にする
  SceneAlarming(std::shared_ptr<hardware::Hardware> &m_hardware)
      : m_hardware(m_hardware) {}

  /// 定期的に (タイマーイベントごとに) 呼ばれる。
  virtual EventResult tick() override {

    auto now_count = m_hardware->shaking().getCount();

    if (now_count >= max_count) {
      m_hardware->speaker().stop(); //音の停止

      //カウントの停止とリセット
      m_hardware->shaking().stopCount();
      m_hardware->shaking().resetCount();

      log_i("SceneAlerming Finish");
      return EventResultKind::Finish;
    }

    int remain_count = max_count - m_hardware->shaking().getCount();
    updateLcd(remain_count);

    return EventResultKind::Continue;
  }

  /// シーンがスタックのトップに来たとき呼ばれる。
  virtual EventResult activated() override {

    log_i("SceneAlerming activated()");

    // LCDのクリア
    M5.Lcd.clear();

    //アラーム音の再生を開始
    m_hardware->speaker().play(hardware::SpeakerManager::Music::Alarm);

    // hardware側の振動回数の初期化(Countを使用するのは一人だと仮定．よそで勝手に操作されると困る)
    m_hardware->shaking().resetCount();
    m_hardware->shaking().startCount();

    return EventResultKind::Continue;
  }

  /// ボタンが押されても何もしないので，ボタン関連overrideはしない

protected:
  std::shared_ptr<hardware::Hardware> m_hardware;

  const int max_count = 5; // 100回振ると，アラーム停止

private:
  void updateLcd(int remain_count) const {
    static int prev_count = 0;
    if (remain_count == prev_count)
      return;
    prev_count = remain_count;
    //描画準備
    //文字色設定
    M5.Lcd.setTextColor(TFT_YELLOW, TFT_BLACK);
    //描画位置
    int xpos = 0;
    int ypos = 85 - 24; // Top left corner ot clock text, about half way down

    //描画処理
    //毎分の時間・分の描画（分が変更していれば、時間は変わっていなくても時間を描画しなおす）
    xpos += M5.Lcd.drawString("Shake!", xpos, ypos, 0); // Draw hours
    xpos = 0;
    ypos += 24;
    xpos += M5.Lcd.drawNumber(remain_count, xpos, ypos, 8); // Draw hours
  }
};

}; // namespace scene
