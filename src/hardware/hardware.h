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
#include "ShakingManager.hpp"

namespace hardware {

class Hardware : protected ButtonManager, protected SpeakerManager ,public ShakingManager{
public:
  Hardware() {}
  void begin() {
    // M5Stack includes LCD, SD, M5.Btn, M5.Speaker,...
    M5.begin();
    // Speaker
    SpeakerManager::begin();
    // Button
    // TODO: これは使用例．実際にやることができたら置き換える
    ButtonManager::onEvent(
        [&](ButtonManager::Button k, ButtonManager::EventKind e) {
          log_d("Button Kind: %s, Event: %s", ButtonManager::c_str(k),
                ButtonManager::c_str(e));
          if (k == ButtonManager::Button::A &&
              e == ButtonManager::EventKind::Pressed)
            SpeakerManager::play(SpeakerManager::Music::Alarm);
          if (k == ButtonManager::Button::B &&
              e == ButtonManager::EventKind::Pressed)
            SpeakerManager::stop();
        });
    ButtonManager::begin();


    // Shaking
    //IMUの初期化とWireの初期化．振動検知タスクの開始
    ShakingManager::begin();
    
  }

private:
};

}; // namespace hardware