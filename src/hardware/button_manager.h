/**
 * @file button.h
 * @author Ryotaro Onuki (kerikun11+github@gmail.com)
 * @brief ボタンを監視してイベントを発生させるクラスを持つ
 * @date 2018-11-29
 */
#pragma once

// for xTaskCreate() etc.
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "button.h"

// for M5.Btn*
#include <M5Stack.h>

namespace hardware {

/// M5Stack のボタンを監視してイベントを発生させるクラス
class ButtonManager {
public:
  /// ボタン名の表示用
  static const char *c_str(Button k) {
    static const char *names[] = {"A", "B", "C"};
    return names[static_cast<size_t>(k)];
  }
  /// ボタンの押され方の表示用
  static const char *c_str(ButtonEventKind e) {
    static const char *names[] = {"Pressed", "Released"};
    return names[static_cast<size_t>(e)];
  }
  /// ボタンイベントのコールバック関数型
  typedef std::function<void(Button, ButtonEventKind)> EventCallback;

  bool isPressed(Button button) {
    switch (button) {
    case Button::A:
      return M5.BtnA.isPressed();
    case Button::B:
      return M5.BtnB.isPressed();
    case Button::C:
      return M5.BtnC.isPressed();
    }
  }
  // イベントコールバックの登録
  void onEvent(EventCallback eventCallback = nullptr) {
    this->eventCallback = eventCallback;
  }
  // イベント待機を開始する
  void begin() {
    // FreeRTOS により task() をバックグラウンドで実行
    const uint16_t stackSize = 4096;
    UBaseType_t uxPriority = 0;
    xTaskCreate(
        [](void *this_obj) { static_cast<ButtonManager *>(this_obj)->task(); },
        "ButtonManager", stackSize, this, uxPriority, NULL);
  }

private:
  // サンプリング周期
  static constexpr int PeriodMillis = 10;
  // コールバック関数のポインタ
  EventCallback eventCallback = nullptr;

  // FreeRTOS によって実行される関数
  void task() {
    // handleEvent の定期実行
    portTickType xLastWakeTime = xTaskGetTickCount();
    while (1) {
      vTaskDelayUntil(&xLastWakeTime, PeriodMillis / portTICK_RATE_MS);
      handleEvent();
    }
    vTaskDelete(NULL);
  }
  // イベントの確認 & コールバック関数の実行
  void handleEvent() {
    // 状態の更新
    M5.BtnA.read();
    M5.BtnB.read();
    M5.BtnC.read();
    // コールバック関数が登録されているか確認
    if (eventCallback == nullptr)
      return;
    // イベントの確認 & コールバック関数の実行
    if (M5.BtnA.wasPressed())
      eventCallback(Button::A, ButtonEventKind::Pressed);
    if (M5.BtnA.wasReleased())
      eventCallback(Button::A, ButtonEventKind::Released);
    if (M5.BtnB.wasPressed())
      eventCallback(Button::B, ButtonEventKind::Pressed);
    if (M5.BtnB.wasReleased())
      eventCallback(Button::B, ButtonEventKind::Released);
    if (M5.BtnC.wasPressed())
      eventCallback(Button::C, ButtonEventKind::Pressed);
    if (M5.BtnC.wasReleased())
      eventCallback(Button::C, ButtonEventKind::Released);
  }
};

}; // namespace hardware
