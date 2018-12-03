#pragma once
#ifndef _INCLUDE_EVENT_HPP_
#define _INCLUDE_EVENT_HPP_

namespace scene {
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
