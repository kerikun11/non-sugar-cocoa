/**
 * @file hardware.h
 * @author Ryotaro Onuki (kerikun11@gmail.com)
 * @brief すべてのハードウェアをまとめたクラスを持つ
 * @version 0.1
 * @date 2018-11-29
 */
#pragma once

// コンパイルエラーを防ぐため， Arduino.h で定義されているマクロをundef
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#include <memory>
#include <utility>

#include "../time_of_day.hpp"
#include "alarm_manager.hpp"
#include "button_manager.h"
#include "shaking_manager.hpp"
#include "speaker_manager.h"
#include "ticker.h"
#include "tweet_manager.h"

#include <M5Stack.h>

namespace hardware {

class Hardware {
private:
  AlarmManager m_alarm;
  ButtonManager m_button;
  SpeakerManager m_speaker;
  Ticker m_ticker;
  ShakingManager m_shaking;
  TweetManager m_tweet;

public:
  /// Thread-local alarm state cache.
  ///
  /// This can be accessed in thread-safe way because it is not shared among
  /// threads.
  thread_local static bool m_alarmIsEnabled;
  /// Thread-local alarm time cache.
  ///
  /// This can be accessed in thread-safe way because it is not shared among
  /// threads.
  thread_local static sugar::TimeOfDay m_alarmTimeCache;

public:
  void begin() {
    // M5Stack includes LCD, SD, M5.Btn, M5.Speaker,...
    M5.begin();
    // Wire
    Wire.begin();
    // Alarm
    m_alarm.begin();
    // Speaker
    m_speaker.begin();
    // Button
    m_button.begin();
    // Shaking
    // IMUの初期化とWireの初期化．振動検知タスクの開始
    m_shaking.begin();
    // Ticker
    m_ticker.begin();
    // Tweet
    m_tweet.begin();
  }
  /// Tickerイベントを割り当てする
  void onTickEvent(Ticker::EventCallback callback) {
    m_ticker.onEvent(callback);
  }
  void onButtonEvent(ButtonManager::EventCallback callback) {
    m_button.onEvent(callback);
  }
  void onAlarmEvent(AlarmManager::EventCallback callback) {
    m_alarm.onEvent(callback);
  }
  /// Alarm manager.
  AlarmManager &alarm() { return m_alarm; }
  /// Button manager.
  ButtonManager &button() { return m_button; }
  /// Speaker manager.
  SpeakerManager &speaker() { return m_speaker; }
  /// Ticker.
  Ticker &ticker() { return m_ticker; }
  /// Shaking manager.
  ShakingManager &shaking() { return m_shaking; }
  /// Tweet manager.
  TweetManager &tweet() { return m_tweet; }

  // Workaround for GCC-5 bug
  // <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66971>.
  static void setAlarmEnabled(bool v);
  // Workaround for GCC-5 bug
  // <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66971>.
  static bool isAlarmEnabled();
  // Workaround for GCC-5 bug
  // <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66971>.
  static void setAlarmTimeCache(sugar::TimeOfDay v);
  // Workaround for GCC-5 bug
  // <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66971>.
  static const sugar::TimeOfDay &alarmTimeCache();

private:
};

}; // namespace hardware
