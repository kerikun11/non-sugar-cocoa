/**
 * @file speaker.h
 * @author Ryotaro Onuki (kerikun11@gmail.com)
 * @brief キューに詰めるとバックグラウンドで音楽を鳴らすSpeakerクラスを持つ
 * @date 2018-11-29
 */
#pragma once

#include <M5Stack.h>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

namespace hardware {

class Speaker {
public:
  /// サウンド
  enum class Music {
    Alarm,
  };
  /// サウンド終了のコールバック関数型
  typedef std::function<void()> Callback;

protected:
  void begin() {
    // キューを初期化
    const int uxQueueLength = 10;
    playlist = xQueueCreate(uxQueueLength, sizeof(void *));
    // FreeRTOS により task() をバックグラウンドで実行
    const uint16_t stackSize = 4096;
    UBaseType_t uxPriority = 0;
    xTaskCreate(
        [](void *this_obj) { static_cast<Speaker *>(this_obj)->task(); },
        "Speaker", stackSize, this, uxPriority, NULL);
  }

  /// 音楽をキュー
  void push(Music m, Callback c = nullptr) {
    auto qi = new QueueItem;
    qi->m = m;
    qi->c = c;
    xQueueSendToBack(playlist, &qi, 0);
  }

private:
  QueueHandle_t playlist;

  struct QueueItem {
    Music m;
    Callback c;
  };

  void task() {
    while (1) {
      QueueItem *qi;
      xQueueReceive(playlist, &qi, portMAX_DELAY);
      log_d("Received a Music");
      // TODO: 再生
      if (qi->c != nullptr)
        qi->c();
      delete qi;
    }
  }
};

}; // namespace hardware