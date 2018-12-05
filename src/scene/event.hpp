#pragma once
#ifndef _INCLUDE_EVENT_HPP_
#define _INCLUDE_EVENT_HPP_

#include <utility>

#include <esp32-hal-log.h>

#include "../hardware/button.h"

namespace scene {
/// イベントの種類。
enum class EventKind {
  /// 一定時間経過を表現するイベント。
  ///
  /// 定期的に発生する。
  Tick,
  /// ボタン関連イベント。
  ///
  /// データ部に `std::unique_ptr<ButtonEvent>` を持つ。
  Button,
  /// アラームイベント。
  Alarm,
};

/// シーンに対するイベント。
class Event {
private:
  /// イベントの種類。
  EventKind m_kind;
  /// 補助データ。
  ///
  /// ポインタの型は `kind` ごとに異なる。
  /// 補助データが不要なら nullptr を使う。
  void *m_data;

public:
  Event(EventKind kind) : m_kind{kind}, m_data{} {}
  Event(EventKind kind, void *data) : m_kind{kind}, m_data{data} {}
  Event(const Event &) = delete;
  Event(Event &&) = default;
  ~Event() {
    if (m_data) {
      log_w("Memory leak detected: the additional data of a scene event was "
            "not released!");
    }
  }

  EventKind kind() const { return m_kind; }

  std::unique_ptr<hardware::ButtonEvent> buttonData() {
    return std::unique_ptr<hardware::ButtonEvent>(
        static_cast<hardware::ButtonEvent *>(std::exchange(m_data, nullptr)));
  }
};

/// イベント処理結果の種類。
enum class EventResultKind {
  /// シーンを続行する。
  Continue,
  /// シーンは終了する。
  ///
  /// スタックのトップのシーンはマネージャによって取り除かれるべきである。
  Finish,
  /// シーンを追加する。
  ///
  /// データ部に `std::unique_ptr<Scene>` を持つ。
  PushScene,
  /// 現在のシーンを入れ替える。
  ///
  /// データ部に `std::unique_ptr<Scene>` を持つ。
  ReplaceScene,
};

/// イベント処理の結果。
struct EventResult {
  /// イベント処理結果の種類。
  EventResultKind kind;
  /// 補助データ。
  ///
  /// ポインタの型は `kind` ごとに異なる。
  /// 補助データが不要なら nullptr を使う。
  void *data;

  EventResult(EventResultKind kind) : kind(kind), data(nullptr) {}
  EventResult(EventResultKind kind, void *data) : kind(kind), data(data) {}
};
} // namespace scene

#endif
