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

#include <AudioFileSourceID3.h>
#include <AudioFileSourceSD.h>
#include <AudioGeneratorMP3.h>
#include <AudioOutputI2S.h>
#include <HTTPClient.h> //< 依存関係コンパイルエラーを防止

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
      if (mp3 && mp3->isRunning()) {
        // 以下の関数しばしばフリーズ
        if (!mp3->loop()) {
          id3->seek(0, SEEK_SET);
        }
      }
      QueueItem *qi;
      if (pdFALSE == xQueueReceive(eventQueue, &qi, 1 / portTICK_PERIOD_MS))
        continue;
      auto item = std::unique_ptr<QueueItem>{qi};
      switch (item->e) {
      case Event::Play:
        log_d("Play");
        // M5.Speaker.beep();
        mp3_start();
        break;
      case Event::Stop:
        log_d("Stop");
        // M5.Speaker.mute();
        mp3_stop();
        break;
      }
    }
  }

public:
  std::unique_ptr<AudioGeneratorMP3> mp3;
  std::unique_ptr<AudioFileSourceSD> file;
  std::unique_ptr<AudioOutputI2S> out;
  std::unique_ptr<AudioFileSourceID3> id3;

  void mp3_start() {
    // static bool initialized = false;
    // if (!initialized) {
    //   initialized = true;
    file = std::make_unique<AudioFileSourceSD>("/bachfugue.mp3");
    id3 = std::make_unique<AudioFileSourceID3>(file.get());
    out = std::make_unique<AudioOutputI2S>(0, 1);
    mp3 = std::make_unique<AudioGeneratorMP3>();
    // }
    out->SetOutputModeMono(true);
    out->SetGain(0.1);
    mp3->begin(id3.get(), out.get());
  }
  void mp3_stop() {
    mp3->stop();
    file.reset();
    id3.reset();
    out.reset();
    mp3.reset();
  }
};

}; // namespace hardware
