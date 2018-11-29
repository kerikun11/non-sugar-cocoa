/**
 * @file hardware.h
 * @author Ryotaro Onuki (kerikun11@gmail.com)
 * @brief すべてのハードウェアをまとめたクラスを持つ
 * @version 0.1
 * @date 2018-11-29
 */
#pragma once

#include <M5Stack.h>

#include "button.h"
#include "speaker.h"

namespace hardware {

class Hardware : protected Button, private Speaker {
public:
  Hardware() {}
  void begin() {
    // M5Stack includes LCD, SD, M5.Btn, M5.Speaker,...
    M5.begin();
    // Button
    // TODO: これは使用例．実際にやることができたら置き換える
    Button::onEvent([](Button::Kind k, Button::EventKind e) {
      log_d("Button Kind: %s, Event: %s", Button::c_str(k), Button::c_str(e));
    });
    Button::begin();
    // Speaker
    Speaker::begin();
    Speaker::push(Speaker::Music::Alarm, []() { log_d("Speaker Callback"); });
    // IMU
    // TODO: IMU
  }

private:
};

}; // namespace hardware