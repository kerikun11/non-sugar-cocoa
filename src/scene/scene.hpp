#pragma once
#ifndef _INCLUDE_SCENE_HPP_
#define _INCLUDE_SCENE_HPP_

#include "../hardware/hardware.h"
#include "event.hpp"

namespace scene {
using Button = hardware::Button;

/// シーンの基底クラス。
class Scene {
public:
  // 基底クラスになるので virtual dtor を使う。
  virtual ~Scene() = default;

  /// 定期的に (タイマーイベントごとに) 呼ばれる。
  virtual EventResult tick() { return EventResultKind::Continue; }

  /// シーンがスタックのトップに来たとき呼ばれる。
  virtual EventResult activated() { return EventResultKind::Continue; }

  /// ボタン関連のイベントが来たとき呼ばれる。
  ///
  /// デフォルト実装では、ここから `buttonAPressed()` などに
  /// 処理が盥回しされる。
  virtual EventResult buttonEventReceived(Button button,
                                          ButtonEventKind event) {
    int button_n = 0;
    switch (button) {
    case Button::A:
      button_n = 1;
      break;
    case Button::B:
      button_n = 2;
      break;
    case Button::C:
      button_n = 3;
      break;
    }
    int event_n = 0;
    switch (event) {
    case ButtonEventKind::Pressed:
      event_n = 1;
      break;
    case ButtonEventKind::Released:
      event_n = 2;
      break;
    case ButtonEventKind::Repeated:
      event_n = 3;
      break;
    }
    int n = event_n << 4 | button_n;
    switch (n) {
    case 0x11:
      return buttonAPressed();
    case 0x12:
      return buttonAReleased();
    case 0x13:
      return buttonARepeated();
    case 0x21:
      return buttonBPressed();
    case 0x22:
      return buttonBReleased();
    case 0x23:
      return buttonBRepeated();
    case 0x31:
      return buttonCPressed();
    case 0x32:
      return buttonCReleased();
    case 0x33:
      return buttonCRepeated();
    default:
      // Should never come here.
      return EventResultKind::Continue;
    }
  }

  /// ボタンAが押されたとき呼ばれる。
  virtual EventResult buttonAPressed() { return EventResultKind::Continue; }

  /// ボタンAが長押しされたとき、定期的に呼ばれる。
  virtual EventResult buttonAReleased() { return EventResultKind::Continue; }

  /// ボタンAが離されたとき呼ばれる。
  virtual EventResult buttonARepeated() { return buttonAPressed(); }

  /// ボタンBが押されたとき呼ばれる。
  virtual EventResult buttonBPressed() { return EventResultKind::Continue; }

  /// ボタンBが長押しされたとき、定期的に呼ばれる。
  virtual EventResult buttonBReleased() { return EventResultKind::Continue; }

  /// ボタンBが離されたとき呼ばれる。
  virtual EventResult buttonBRepeated() { return buttonBPressed(); }

  /// ボタンCが押されたとき呼ばれる。
  virtual EventResult buttonCPressed() { return EventResultKind::Continue; }

  /// ボタンCが長押しされたとき、定期的に呼ばれる。
  virtual EventResult buttonCReleased() { return EventResultKind::Continue; }

  /// ボタンCが離されたとき呼ばれる。
  virtual EventResult buttonCRepeated() { return buttonCPressed(); }
};
} // namespace scene

#endif
