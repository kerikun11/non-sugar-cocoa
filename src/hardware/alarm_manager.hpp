#pragma once
#ifndef _INCLUDE_ALARM_MANAGER_HPP_
#define _INCLUDE_ALARM_MANAGER_HPP_

#include <atomic>
#include <utility>

#include <esp32-hal-log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include "../time_of_day.hpp"

namespace hardware {
/// アラーム時刻設定機構。
class AlarmTimeSetter {
private:
  QueueHandle_t m_queue;

public:
  AlarmTimeSetter(QueueHandle_t q) : m_queue(q) {}
  AlarmTimeSetter(const AlarmTimeSetter &) = default;
  AlarmTimeSetter(AlarmTimeSetter &&) = default;
  AlarmTimeSetter &operator=(const AlarmTimeSetter &) = default;
  AlarmTimeSetter &operator=(AlarmTimeSetter &&) = default;

  /// 時刻を設定する。
  ///
  /// 正しく設定コマンドが送信されたとき `true` を返す。
  bool setAlarmTime(const sugar::TimeOfDay &time) {
    auto obj = std::make_unique<sugar::TimeOfDay>(time);
    sugar::TimeOfDay *ptr = obj.release();
    return xQueueSendToBack(m_queue, &ptr, 0) == pdTRUE;
  }
};

/// アラームイベント発生を担当するクラス。
class AlarmManager {
public:
  /// アラームイベントのコールバック関数型。
  typedef std::function<void()> EventCallback;

private:
  /// アラームが有効化されているか否か。
  std::atomic_bool m_alarmIsSet;
  /// アラーム設定時刻。
  sugar::TimeOfDay m_alarmTime;
  /// コールバック関数。
  EventCallback eventCallback = nullptr;
  /// アラーム時刻設定用キュー。
  QueueHandle_t m_timeSetterQueue;

  /// Tick 周期。
  static constexpr int PeriodMillis = 100;
  /// キューのサイズ。
  static constexpr int TIME_SETTER_QUEUE_SIZE = 16;

public:
  /// イベントコールバックの登録
  void onEvent(EventCallback eventCallback = nullptr) {
    this->eventCallback = eventCallback;
  }

  /// イベント待機を開始する
  void begin() {
    // FreeRTOS により task() をバックグラウンドで実行
    const uint16_t stackSize = 4096;
    // タスクの優先度
    constexpr UBaseType_t uxPriority = 0;
    xTaskCreate(
        [](void *this_obj) { static_cast<AlarmManager *>(this_obj)->task(); },
        "AlarmManagre", stackSize, this, uxPriority, NULL);
    m_timeSetterQueue =
        xQueueCreate(TIME_SETTER_QUEUE_SIZE, sizeof(sugar::TimeOfDay *));
  }

  /// アラームが設定されているかどうかを返す。
  bool isAlarmSet() const { return m_alarmIsSet; }

  /// アラーム設定用オブジェクトを返す。
  // 時刻は別スレッドから設定されることがありうるため、競合を起こさないように
  // する必要がある。
  AlarmTimeSetter alarmTimeSetter() {
    return AlarmTimeSetter(m_timeSetterQueue);
  }

private:
  // FreeRTOS によって実行される関数
  void task() {
    portTickType xLastWakeTime = xTaskGetTickCount();
    while (1) {
      // 定期実行
      vTaskDelayUntil(&xLastWakeTime, PeriodMillis / portTICK_RATE_MS);

      // アラーム時刻変更コマンドを処理
      {
        sugar::TimeOfDay *qi;
        while (xQueueReceive(m_timeSetterQueue, &qi, 0) == pdTRUE) {
          auto time =
              std::unique_ptr<sugar::TimeOfDay>{std::exchange(qi, nullptr)};
          log_i("AlarmManager::task(): Alarm time updated: %02d:%02d:%02d",
                time->hour(), time->minute(), time->second());
          m_alarmTime = std::move(*time);
          m_alarmIsSet = true;
        }
      }

      // アラームとコールバックがともに設定されていれば、時刻を確認
      if (isAlarmSet() && eventCallback) {
        auto current =
            sugar::TimeOfDay::fromUtcToJst(std::chrono::system_clock::now());
        // 設定された時間以降 990 ミリ秒以内であればイベント発火
        if (current.isAfter(m_alarmTime, std::chrono::milliseconds(990))) {
          eventCallback();
          // Unset alarm.
          m_alarmIsSet = false;
        }
      }
    }
  }
};
} // namespace hardware
#endif
