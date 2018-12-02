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

    auto now_count = m_hardware->getCount();

    if (now_count >= max_count) {
      m_hardware->stop(); //音の停止

      //カウントの停止とリセット
      m_hardware->stopCount();
      m_hardware->resetCount();

      log_i("SceneAlerming Finish");
      return EventResultKind::Finish;
    }

    return EventResultKind::Continue;
  }

  /// シーンがスタックのトップに来たとき呼ばれる。
  virtual EventResult activated() override {

    log_i("SceneAlerming activated()");

    //アラーム音の再生を開始
    m_hardware->play(hardware::Hardware::SpeakerManager::Music::Alarm);

    // hardware側の振動回数の初期化(Countを使用するのは一人だと仮定．よそで勝手に操作されると困る)
    m_hardware->resetCount();
    m_hardware->startCount();

    return EventResultKind::Continue;
  }

  /// ボタンが押されても何もしないので，ボタン関連overrideはしない

protected:
  std::shared_ptr<hardware::Hardware> m_hardware;

  const int max_count = 100; // 100回振ると，アラーム停止
};                           // namespace scene
};                           // namespace scene
