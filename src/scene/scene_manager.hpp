#pragma once
#ifndef _INCLUDE_SCENE_MANAGER_HPP_
#define _INCLUDE_SCENE_MANAGER_HPP_

#include <stdexcept>
#include <vector>

#include "../hardware/button_manager.h"
#include "../hardware/hardware.h"
#include "scene.hpp"
#include "scene_clock.hpp"
#include "scene_set_clock.hpp"

namespace scene {
/// シーン管理機構。
class SceneManager {
private:
  /// シーンのスタック。
  std::vector<std::unique_ptr<Scene>> m_scenes;
  /// ハードウェア状態の参照。
  std::shared_ptr<hardware::Hardware> m_hardware;

public:
  /// シーン管理機構を初期化。
  void initialize(const std::shared_ptr<hardware::Hardware> &hardware) {
    // ハードウェア管理機構を記憶する。
    m_hardware = hardware;
    // イベントを割り当てる
    m_hardware->onButtonEvent(
        [&](hardware::Button button, hardware::ButtonEventKind event) {
          log_d("Button: %s, Event: %s", hardware::ButtonManager::c_str(button),
                hardware::ButtonManager::c_str(event));
          updateStack(m_scenes.back()->buttonEventReceived(button, event));
        });
    m_hardware->onTickEvent([&]() { updateStack(m_scenes.back()->tick()); });
    // 初期状態は時刻表示。
    m_scenes.push_back(std::unique_ptr<SceneClock>(new SceneClock(m_hardware)));
    updateStack(m_scenes.back()->activated());
  }

protected:
  /// シーンのイベント処理結果を受けてスタックを更新する。
  void updateStack(EventResult result) {
    switch (result.kind) {
    case EventResultKind::Continue:
      // Do nothing.
      break;
    case EventResultKind::Finish:
      m_scenes.pop_back();
      break;
    case EventResultKind::PushScene: {
      std::unique_ptr<Scene> scene(static_cast<Scene *>(result.data));
      m_scenes.push_back(std::move(scene));
      updateStack(m_scenes.back()->activated());
    } break;
    case EventResultKind::ReplaceScene:
      break;
    }
  }
};
} // namespace scene

#endif
