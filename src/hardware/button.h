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

}; // namespace hardware
