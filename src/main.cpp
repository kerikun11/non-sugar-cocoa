/**
 * @file main.cpp
 * @author Ryotaro Onuki (kerikun11@gmail.com)
 * @brief アプリケーションの起点はこのファイルに書く
 * @date 2018-11-29
 */
#include <esp32-hal-log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include "hardware/button.h"
#include "hardware/hardware.h"
#include "scene/event.hpp"
#include "scene/scene_boot.hpp"
#include "scene/scene_manager.hpp"

/// グローバル変数
std::shared_ptr<hardware::Hardware> hw;
scene::SceneManager scene_manager;
scene::SceneEventSender sceneEventSender;

void setup() {
  // put your setup code here, to run once:
  log_i("Hello World!");

  // ハードウェアの初期化
  hw = std::make_shared<hardware::Hardware>();
  hw->begin();

  // シーン管理機構を初期化。
  constexpr int EVENT_QUEUE_LENGTH = 128;
  log_i("Scene manager event queue size = %d", EVENT_QUEUE_LENGTH);
  // シーンイベントキューハンドラを作成
  QueueHandle_t queueToSceneManager =
      xQueueCreate(EVENT_QUEUE_LENGTH, sizeof(scene::Event *));
  auto initial_scene = std::unique_ptr<scene::Scene>(new scene::SceneBoot(hw));
  scene_manager.initialize(hw, queueToSceneManager, std::move(initial_scene));
  sceneEventSender = scene::SceneEventSender{queueToSceneManager};

  // ハードウェア関係の設定。
  hw->onTickEvent([=]() { sceneEventSender.tick(); });
  hw->onButtonEvent([=](hardware::Button bt, hardware::ButtonEventKind btk) {
    sceneEventSender.button(ButtonEvent{bt, btk});
  });
}

void loop() {
  // put your main code here, to run repeatedly:
  size_t numEventsProcessed = scene_manager.processExternalEvents();
  if (numEventsProcessed != 0) {
    log_i("Processed %zd external events by scene manager", numEventsProcessed);
  }
}
