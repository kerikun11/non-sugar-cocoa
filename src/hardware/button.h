#pragma once

namespace hardware {

/// ボタンの種類
enum class Button { A, B, C };

/// ボタンの押され方
enum class ButtonEventKind {
  /// Pressed.
  Pressed,
  /// Released.
  Released,
  /// Being pressed for a while.
  Repeated,
};

/// ボタンのイベント
struct ButtonEvent {
  /// ボタン
  Button button;
  /// ボタンの押され方
  ButtonEventKind kind;

  ButtonEvent(Button b, ButtonEventKind k) : button(b), kind(k) {}
};

}; // namespace hardware
