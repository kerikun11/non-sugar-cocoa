/**
 * @file button.h
 * @author Ryotaro Onuki (kerikun11+github@gmail.com)
 * @brief ボタンを監視してイベントを発生させるクラスを持つ
 * @date 2018-11-29
 */
#pragma once

// for M5.Btn*
#include <M5Stack.h>
// for xTaskCreate() etc.
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace hardware {

/// M5Stack のボタンを監視してイベントを発生させるクラス
class Button {
public:
  /// ボタンの種類
  enum class Kind { A, B, C };
  /// ボタン名の表示用
  static const char *c_str(Kind k);
  /// ボタンの押され方
  enum class EventKind { Pressed, Released };
  /// ボタンの押され方の表示用
  static const char *c_str(EventKind e);
  /// ボタンイベントのコールバック関数型
  typedef std::function<void(Kind, EventKind)> EventCallback;

protected:
  // イベントコールバックの登録
  void onEvent(EventCallback eventCallback = nullptr);
  // イベント待機を開始する
  void begin();

private:
  // サンプリング周期
  static constexpr int PeriodMillis = 10;
  // コールバック関数のポインタ
  EventCallback eventCallback = nullptr;

  // FreeRTOS によって実行される関数
  void task();
  // イベントの確認 & コールバック関数の実行
  void handleEvent();
};

const char *Button::c_str(Kind k) {
  static const char *names[] = {"A", "B", "C"};
  return names[static_cast<size_t>(k)];
}
const char *Button::c_str(EventKind e) {
  static const char *names[] = {"Pressed", "Released"};
  return names[static_cast<size_t>(e)];
}
void Button::onEvent(EventCallback eventCallback) {
  this->eventCallback = eventCallback;
}
void Button::begin() {
  // FreeRTOS により task() をバックグラウンドで実行
  const uint16_t stackSize = 4096;
  UBaseType_t uxPriority = 0;
  xTaskCreate([](void *this_obj) { static_cast<Button *>(this_obj)->task(); },
              "Button", stackSize, this, uxPriority, NULL);
}
void Button::task() {
  // handleEvent の定期実行
  portTickType xLastWakeTime = xTaskGetTickCount();
  while (1) {
    vTaskDelayUntil(&xLastWakeTime, PeriodMillis / portTICK_RATE_MS);
    handleEvent();
  }
  vTaskDelete(NULL);
}
void Button::handleEvent() {
  // 状態の更新
  M5.BtnA.read();
  M5.BtnB.read();
  M5.BtnC.read();
  // コールバック関数が登録されているか確認
  if (eventCallback == nullptr)
    return;
  // イベントの確認 & コールバック関数の実行
  if (M5.BtnA.wasPressed())
    eventCallback(Button::Kind::A, Button::EventKind::Pressed);
  if (M5.BtnA.wasReleased())
    eventCallback(Button::Kind::A, Button::EventKind::Released);
  if (M5.BtnB.wasPressed())
    eventCallback(Button::Kind::B, Button::EventKind::Pressed);
  if (M5.BtnB.wasReleased())
    eventCallback(Button::Kind::B, Button::EventKind::Released);
  if (M5.BtnC.wasPressed())
    eventCallback(Button::Kind::C, Button::EventKind::Pressed);
  if (M5.BtnC.wasReleased())
    eventCallback(Button::Kind::C, Button::EventKind::Released);
}

}; // namespace hardware