#pragma once

#include <M5Stack.h>

class Hardware {
public:
  /// ボタンの種類
  enum class ButtonKind { A, B, C };
  /// ボタン名の表示用
  static const char *c_str(ButtonKind k) {
    static const char *names[] = {"A", "B", "C"};
    return names[static_cast<size_t>(k)];
  }
  /// ボタンの押され方
  enum class ButtonEventKind { Pressed, Released };
  /// ボタンの押され方の表示用
  static const char *c_str(ButtonEventKind e) {
    static const char *names[] = {"Pressed", "Released"};
    return names[static_cast<size_t>(e)];
  }
  /// ボタンイベントのコールバック関数型
  typedef std::function<void(ButtonKind buttonKind,
                             ButtonEventKind buttonEventKind)>
      ButtonEventCallback;

public:
  Hardware() {}
  void onButtonEvent(ButtonEventCallback buttonEventCallback) {
    this->buttonEventCallback = buttonEventCallback;
  }
  void begin() {
    M5.begin();
    xTaskCreate([](void *obj) { static_cast<Hardware *>(obj)->task(); },
                "Hardware", 4096, this, 0, NULL);
  }

private:
  ButtonEventCallback buttonEventCallback = nullptr;

  void handleButtonEvent() {
    if (buttonEventCallback == nullptr)
      return;
    if (M5.BtnA.wasPressed())
      buttonEventCallback(ButtonKind::A, ButtonEventKind::Pressed);
    if (M5.BtnA.wasReleased())
      buttonEventCallback(ButtonKind::A, ButtonEventKind::Released);
    if (M5.BtnB.wasPressed())
      buttonEventCallback(ButtonKind::B, ButtonEventKind::Pressed);
    if (M5.BtnB.wasReleased())
      buttonEventCallback(ButtonKind::B, ButtonEventKind::Released);
    if (M5.BtnC.wasPressed())
      buttonEventCallback(ButtonKind::C, ButtonEventKind::Pressed);
    if (M5.BtnC.wasReleased())
      buttonEventCallback(ButtonKind::C, ButtonEventKind::Released);
  }
  void task() {
    while (1) {
      delay(1);
      M5.update();
      handleButtonEvent();
    }
  }
};
