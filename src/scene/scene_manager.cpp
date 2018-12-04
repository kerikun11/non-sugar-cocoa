#include <memory>
#include <utility>

#include "scene_alarming.hpp"
#include "scene_manager.hpp"

namespace scene {
size_t SceneManager::processExternalEvents() {
  size_t num_msgs = uxQueueMessagesWaiting(m_eventReceiver);
  if (num_msgs > 0) {
    log_d("SceneManager::processExternalEvent: %zd events to be processed",
          num_msgs);
  }
  for (size_t i = 0; i < num_msgs; ++i) {
    Event *qi;
    // This will not block so long, because there should be at least one
    // message rest.
    xQueueReceive(m_eventReceiver, &qi, portMAX_DELAY);
    // The item should be created by `new Event(foobar)`,
    // so that it can be safely `delete`d.
    auto ev = std::unique_ptr<Event>{qi};

    bool isAlarmEvent = false;
    {
      // Pass the event to the top (currently active) scene.
      auto &currentScene = *m_scenes.back();
      switch (ev->kind()) {
      case EventKind::Tick:
        updateStack(currentScene.tick());
        break;
      case EventKind::Button: {
        auto bte = ev->buttonData();
        updateStack(currentScene.buttonEventReceived(bte->button, bte->kind));
      } break;
      case EventKind::Alarm:
        // 一応送信しておく。何かに使うかもしれないし。
        updateStack(currentScene.alarm());
        // アラーム画面を強制的に有効化するのは、各シーンでなくマネージャの
        // 責任とする。
        // ここで `m_scenes.push_back()` すると、 `currentScene` が dangling
        // してしまうのでアカン
        isAlarmEvent = true;
        break;
      }
    }
    if (isAlarmEvent) {
      auto alarmScene = std::unique_ptr<Scene>(new SceneAlarming(m_hardware));
      m_scenes.push_back(std::move(alarmScene));
      updateStack(m_scenes.back()->activated());
    }
  }

  return num_msgs;
}
} // namespace scene
