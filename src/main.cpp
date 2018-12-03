/**
 * @file main.cpp
 * @author Ryotaro Onuki (kerikun11@gmail.com)
 * @brief アプリケーションの起点はこのファイルに書く
 * @date 2018-11-29
 */
#include <esp32-hal-log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include "hardware/hardware.h"
#include "scene/event.hpp"
#include "scene/scene_manager.hpp"

std::shared_ptr<hardware::Hardware> hw;
scene::SceneManager scene_manager;

void setup() {
  // put your setup code here, to run once:
  log_i("Hello World!");
  constexpr int EVENT_QUEUE_LENGTH = 128;
  hw = std::make_shared<hardware::Hardware>();
  hw->begin();

  // シーン管理機構を初期化。
  log_i("Scene manager event queue size = %d", EVENT_QUEUE_LENGTH);
  QueueHandle_t queueToSceneManager =
      xQueueCreate(EVENT_QUEUE_LENGTH, sizeof(scene::Event *));
  scene_manager.initialize(hw, queueToSceneManager);

  // ハードウェア関係の設定。
  hw->onTickEvent([=]() {
    scene::Event *ev = new scene::Event{scene::EventKind::Tick};
    xQueueSendToBack(queueToSceneManager, ev, 0);
  });
}

void loop() {
  // put your main code here, to run repeatedly:
  size_t numEventsProcessed = scene_manager.processExternalEvents();
  if (numEventsProcessed != 0) {
    log_i("Processed %zd external events by scene manager", numEventsProcessed);
  }
}
