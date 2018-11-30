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

namespace hardware {

class Hardware : protected ButtonManager, protected SpeakerManager {
public:
  Hardware() {}
  void begin() {
    // M5Stack includes LCD, SD, M5.Btn, M5.Speaker,...
    M5.begin();
    //時計合わせ
    initClock();
    // Speaker
    SpeakerManager::begin();
    // Button
    // TODO: これは使用例．実際にやることができたら置き換える
    ButtonManager::onEvent([&](Button k, ButtonManager::EventKind e) {
      log_d("Button Kind: %s, Event: %s", ButtonManager::c_str(k),
            ButtonManager::c_str(e));
      if (k == Button::A && e == ButtonManager::EventKind::Pressed){
        SpeakerManager::play(SpeakerManager::Music::Alarm);
      }
      if (k == Button::B && e == ButtonManager::EventKind::Pressed){
        SpeakerManager::stop();
      }
      if(k==Button::C && e==ButtonManager::EventKind::Pressed){
        drawClock();
      }
    });
    ButtonManager::begin();
    // IMU
    // TODO: IMU
  }

private:
  void drawClock()const;
  void connectWiFi()const;
  void disconnectWiFi()const;
  void initClock()const;
};

}; // namespace hardware