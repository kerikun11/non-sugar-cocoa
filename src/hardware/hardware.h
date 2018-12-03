/**
 * @file hardware.h
 * @author Ryotaro Onuki (kerikun11@gmail.com)
 * @brief すべてのハードウェアをまとめたクラスを持つ
 * @version 0.1
 * @date 2018-11-29
 */
#pragma once

#include <M5Stack.h>

#include "shaking_manager.hpp"
#include "button_manager.h"
#include "speaker_manager.h"
#include "ticker.h"

#include <memory>

namespace hardware {

class Hardware : public ButtonManager,
                 public SpeakerManager,
                 public Ticker,
                 public ShakingManager {

public:
  Hardware() {}
  void begin() {
    // M5Stack includes LCD, SD, M5.Btn, M5.Speaker,...
    M5.begin();
    //時計合わせ
    ntpInit();
    // Speaker
    SpeakerManager::begin();
    // Button
    ButtonManager::begin();

    // Shaking
    // IMUの初期化とWireの初期化．振動検知タスクの開始
    ShakingManager::begin();

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
  /// WiFi接続(ブロッキング)
  void connectWiFi() const;
  /// WiFi切断
  void disconnectWiFi() const;

private:
  /// 時計合わせ(ブロッキング)
  void ntpInit() const;
};

}; // namespace hardware