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
#include "ticker.h"

namespace hardware {

class Hardware : protected ButtonManager,
                 protected SpeakerManager,
                 protected Ticker {
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
    ButtonManager::begin();
    // IMU
    // TODO: IMU
    // Ticker
    Ticker::begin();
  }
  /// Tickerイベントを割り当てする
  void onTickEvent(Ticker::EventCallback callback) {
    Ticker::onEvent(callback);
  }
  void onButtonEvent(ButtonManager::EventCallback callback) {
    ButtonManager::onEvent(callback);
  }
  void onTickEvent(std::function<void()> callback) {}

private:
  void drawClock() const;
  void connectWiFi() const;
  void disconnectWiFi() const;
  void initClock() const;
};

}; // namespace hardware