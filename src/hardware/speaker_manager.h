/**
 * @file speaker.h
 * @author Ryotaro Onuki (kerikun11@gmail.com)
 * @brief キューに詰めるとバックグラウンドで音楽を鳴らすSpeakerクラスを持つ
 * @date 2018-11-29
 */
#pragma once

#include <M5Stack.h>

#include <memory>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

namespace hardware {

class SpeakerManager {
public:
  /// サウンド
  enum class Music {
    Alarm,
  };
  /// 音楽をキュー
  void play(Music m = Music::Alarm) {
    auto qi = new QueueItem;
    qi->e = Event::Play;
    qi->m = m;
    xQueueSendToBack(eventQueue, &qi, 0);
  }
  void stop() {
    auto qi = new QueueItem;
    qi->e = Event::Stop;
    xQueueSendToBack(eventQueue, &qi, 0);
  }
  void begin() {
    // キューを初期化
    const int uxQueueLength = 10;
    eventQueue = xQueueCreate(uxQueueLength, sizeof(void *));
    // FreeRTOS により task() をバックグラウンドで実行
    const uint16_t stackSize = 4096;
    UBaseType_t uxPriority = 0;
    xTaskCreate(
        [](void *this_obj) { static_cast<SpeakerManager *>(this_obj)->task(); },
        "SpeakerManager", stackSize, this, uxPriority, NULL);
  }

private:
  QueueHandle_t eventQueue;
  // サンプリング周期
  static constexpr int PeriodMillis = 100;

  enum class Event {
    Play,
    Stop,
  };
  struct QueueItem {
    Event e;
    Music m;
  };

  Event state = Event::Stop; //< 再生状態

  // FreeRTOS によって実行される関数
  void task() {
    while (1) {
      // 点滅した音を鳴らす
      // waitの間はイベント待ちなので，イベントを取り損ねることはない．
      if (state == Event::Play)
        M5.Speaker.beep();
      waitWithHandleQueueEvent(50);
      if (state == Event::Play)
        M5.Speaker.mute();
      waitWithHandleQueueEvent(950);
    }
    vTaskDelete(NULL);
  }
  void waitWithHandleQueueEvent(int wait_ms) {
    QueueItem *qi;
    if (pdTRUE != xQueueReceive(eventQueue, &qi, wait_ms / portTICK_PERIOD_MS))
      return;
    auto item = std::unique_ptr<QueueItem>{qi};
    state = item->e;
    switch (item->e) {
    case Event::Play:
      log_d("Play");
      break;
    case Event::Stop:
      log_d("Stop");
      M5.Speaker.mute();
      break;
    }
  }
};

}; // namespace hardware
