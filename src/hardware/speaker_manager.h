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

protected:
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

  enum class Event {
    Play,
    Stop,
  };
  struct QueueItem {
    Event e;
    Music m;
  };

  // FreeRTOS によって実行される関数
  void task() {
    while (1) {
      QueueItem *qi;
      xQueueReceive(eventQueue, &qi, portMAX_DELAY);
      auto item = std::unique_ptr<QueueItem>{qi};
      switch (item->e) {
      case Event::Play:
        log_d("Play");
        M5.Speaker.beep();
        break;
      case Event::Stop:
        log_d("Stop");
        M5.Speaker.mute();
        break;
      }
    }
  }
};

}; // namespace hardware