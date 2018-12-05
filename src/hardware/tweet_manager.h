/**
 * @file speaker.h
 * @author Sekihara Takeshi (seki.maq.kan@gmail.com)
 * @brief キューに詰めるとバックグラウンドでTweetするクラス
 * @date 2018-12-05
 */
#pragma once

#include <M5Stack.h>
#include <memory>

#include "../../lib/stewgate_u/stewgate_u.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <string>

namespace hardware {

class TweetManager {
public:
  TweetManager() : Tweeter("a7405aee08448afb86bd34e903e5548f") {}

  /// tweetしたい内容をキューに追加
  void tweet(std::string str = "") {
    auto qi = new QueueItem;
    qi->str = str;
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
        [](void *this_obj) { static_cast<TweetManager *>(this_obj)->task(); },
        "TweetManager", stackSize, this, uxPriority, NULL);
  }

private:
  QueueHandle_t eventQueue;

  //こいつのtweet関数使ってtweetする
  StewGate_U Tweeter;

  struct QueueItem {
    std::string str;
  };

  // FreeRTOS によって実行される関数
  void task() {
    while (1) {
      QueueItem *qi;
      //このReceiveは中身が無ければ，待ちをする
      xQueueReceive(eventQueue, &qi, portMAX_DELAY);
      auto item = std::unique_ptr<QueueItem>{qi};

      if (WiFi.waitForConnectResult() == WL_CONNECTED) {
        Tweeter.tweet(item->str);
      } else {
        log_e("WiFi Error");
        //再度同じ文面をQueueに追加
        tweet(item->str);
      }
    }
  }
};

}; // namespace hardware
