#pragma once
#ifndef _INCLUDE_SCENE_MANAGER_HPP_
#define _INCLUDE_SCENE_MANAGER_HPP_

// コンパイルエラーを防ぐため， Arduino.h で定義されているマクロをundef
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#include <cstdlib>
#include <stdexcept>
#include <utility>
#include <vector>

#include <esp32-hal-log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include "../hardware/button.h"
#include "../hardware/button_manager.h"
#include "../hardware/hardware.h"
#include "scene.hpp"

using hardware::ButtonEvent;

namespace scene {
/// シーン管理機構。
class SceneManager {
private:
  /// シーンのスタック。
  std::vector<std::unique_ptr<Scene>> m_scenes;
  /// ハードウェア状態の参照。
  std::shared_ptr<hardware::Hardware> m_hardware;
  /// `SceneManager` への(外部から送られてくる)イベントを受信するためのキュー。
  QueueHandle_t m_eventReceiver;

public:
  /// シーン管理機構を初期化。
  void initialize(const std::shared_ptr<hardware::Hardware> &hardware,
                  QueueHandle_t event_receiver,
                  std::unique_ptr<Scene> initial_scene) {
    // ハードウェア管理機構を記憶する。
    m_hardware = hardware;
    // 外部イベント受信キューを記憶する。
    m_eventReceiver = event_receiver;
    // 初期シーンを追加
    m_scenes.push_back(std::move(initial_scene));
    // 初期シーンを始動
    updateStack(m_scenes.back()->activated());
  }

  /// 外部イベント受信キューの内容物をいくつか処理する。
  ///
  /// この関数を呼び出した時点でキューに溜まっていたイベントが処理される。
  ///
  /// 処理されたイベントの個数を返す。
  size_t processExternalEvents();

private:
  /// シーンのイベント処理結果を受けてスタックを更新する。
  void updateStack(EventResult result) {
    switch (result.kind) {
    case EventResultKind::Continue:
      // Do nothing.
      break;
    case EventResultKind::Finish:
      m_scenes.pop_back();
      updateStack(m_scenes.back()->activated());
      break;
    case EventResultKind::PushScene: {
      std::unique_ptr<Scene> scene(static_cast<Scene *>(result.data));
      pushScene(std::move(scene));
    } break;
    case EventResultKind::ReplaceScene: {
      std::unique_ptr<Scene> scene(static_cast<Scene *>(result.data));
      m_scenes.pop_back();
      pushScene(std::move(scene));
    } break;
    }
  }
  /// シーンをスタックに追加する。
  void pushScene(std::unique_ptr<Scene> scene) {
    m_scenes.push_back(std::move(scene));
    updateStack(m_scenes.back()->activated());
  }
};

/// `SceneManager` へイベントを送信するための送信器。
class SceneEventSender {
private:
  QueueHandle_t m_queue;

public:
  SceneEventSender() = default;
  SceneEventSender(QueueHandle_t q) : m_queue{q} {}

  /// 生イベントを送信する。
  void send(std::unique_ptr<Event> ev) {
    Event *p = ev.release();
    xQueueSendToBack(m_queue, &p, 0);
  }

  /// Tick イベントを送信する。
  void tick() {
    auto ev = std::make_unique<Event>(EventKind::Tick);
    send(std::move(ev));
  }

  /// Button イベントを送信する。
  void button(ButtonEvent bte) {
    auto ev = std::make_unique<Event>(
        EventKind::Button, std::make_unique<ButtonEvent>(bte).release());
    send(std::move(ev));
  }

  /// Alarm イベントを送信する。
  void alarm() {
    auto ev = std::make_unique<Event>(EventKind::Alarm);
    send(std::move(ev));
  }
};
} // namespace scene

#endif
