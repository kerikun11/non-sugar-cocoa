#pragma once
#ifndef _INCLUDE_SCENE_MANAGER_HPP_
#define _INCLUDE_SCENE_MANAGER_HPP_

#include <stdexcept>
#include <vector>

#include "../hardware/button_manager.h"
#include "../hardware/hardware.h"
#include "scene.hpp"

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
    // 初期状態は時刻表示。
    // m_scenes.push(std::unique_ptr(new ClockScene(m_hardware)));
    throw std::runtime_error(
        "Unimplemened: Scene for clock is not yet defined");
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
    } break;
    case EventResultKind::ReplaceScene:
      break;
    }
  }
};
} // namespace scene

#endif
