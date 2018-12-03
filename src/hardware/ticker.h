/**
 * @file ticker.h
 * @author Ryotaro Onuki (kerikun11@gmail.com)
 * @brief tick
 * @version 0.1
 * @date 2018-12-01
 */
#pragma once

#include <functional>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace hardware {

/// M5Stack のボタンを監視してイベントを発生させるクラス
class Ticker {
public:
  /// ボタンイベントのコールバック関数型
  typedef std::function<void()> EventCallback;

  // イベントコールバックの登録
  void onEvent(EventCallback eventCallback = nullptr) {
    this->eventCallback = eventCallback;
  }
  // イベント待機を開始する
  void begin() {
    // FreeRTOS により task() をバックグラウンドで実行
    const uint16_t stackSize = 4096;
    UBaseType_t uxPriority = 0;
    xTaskCreate([](void *this_obj) { static_cast<Ticker *>(this_obj)->task(); },
                "Ticker", stackSize, this, uxPriority, NULL);
  }

private:
  // Tick 周期
  static constexpr int PeriodMillis = 100;
  // コールバック関数のポインタ
  EventCallback eventCallback = nullptr;

  // FreeRTOS によって実行される関数
  void task() {
    portTickType xLastWakeTime = xTaskGetTickCount();
    while (1) {
      // 定期実行
      vTaskDelayUntil(&xLastWakeTime, PeriodMillis / portTICK_RATE_MS);
      eventCallback();
    }
  }
};

}; // namespace hardware
