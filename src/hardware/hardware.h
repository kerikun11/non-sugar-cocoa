/**
 * @file hardware.h
 * @author Ryotaro Onuki (kerikun11@gmail.com)
 * @brief すべてのハードウェアをまとめたクラスを持つ
 * @version 0.1
 * @date 2018-11-29
 */
#pragma once

#include <M5Stack.h>

#include "button_manager.h"
#include "speaker_manager.h"

#include"scene/scene_set.hpp"
#include<memory>

namespace hardware {

class Hardware : protected ButtonManager, protected SpeakerManager {
public:
  Hardware() {}
  void begin() {
    // M5Stack includes LCD, SD, M5.Btn, M5.Speaker,...
    M5.begin();
    // Speaker
    SpeakerManager::begin();
    //Scene
    m_scene=std::shared_ptr<scene::SceneSetClock>(new scene::SceneSetClock());
    m_scene->initialize();
    m_scene->update();
    // Button
    // TODO: これは使用例．実際にやることができたら置き換える
    ButtonManager::onEvent([&](Button k, ButtonManager::EventKind e) {
      if (k == Button::A && e == ButtonManager::EventKind::Pressed){
        m_scene->decrement();
      }
      if (k == Button::B && e == ButtonManager::EventKind::Pressed){
        m_scene->proceedProcess();
      }
      if (k == Button::C && e == ButtonManager::EventKind::Pressed){
        m_scene->inclement();
      }
    });
    ButtonManager::begin();
    // IMU
    // TODO: IMU
  }

private:
  std::shared_ptr<scene::SceneSetClock> m_scene;
};

}; // namespace hardware