#pragma once
#ifndef _INCLUDE_TIME_OF_DAY_HPP_
#define _INCLUDE_TIME_OF_DAY_HPP_

// コンパイルエラーを防ぐため， Arduino.h で定義されているマクロをundef
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#include <chrono>
#include <cstdint>

namespace sugar {

/// Time of day.
///
/// タイムゾーン情報は持たない。
class TimeOfDay {
private:
  /// 深夜からの時間。
  std::chrono::milliseconds m_timeSinceMidnight;

  static constexpr auto JST_OFFSET = std::chrono::hours(9);

public:
  TimeOfDay() = default;
  TimeOfDay(const TimeOfDay &) = default;
  TimeOfDay(TimeOfDay &&) = default;
  TimeOfDay &operator=(const TimeOfDay &) = default;
  TimeOfDay &operator=(TimeOfDay &&) = default;

  /// タイムゾーンを考慮しない素朴な初期化。
  template <typename Clock, typename Duration>
  TimeOfDay(std::chrono::time_point<Clock, Duration> time)
      : m_timeSinceMidnight(
            std::chrono::duration_cast<decltype(m_timeSinceMidnight)>(
                modulo24h(time.time_since_epoch()))) {}

  /// タイムゾーンを考慮しない素朴な初期化。
  template <typename Rep, typename Period>
  TimeOfDay(std::chrono::duration<Rep, Period> time)
      : m_timeSinceMidnight(
            std::chrono::duration_cast<decltype(m_timeSinceMidnight)>(
                modulo24h(time))) {}

  /// 協定世界時を受け取り、日本標準時で `TimeOfDay` を作成する。
  template <typename Clock, typename Duration>
  static TimeOfDay fromUtcToJst(std::chrono::time_point<Clock, Duration> utc) {
    return TimeOfDay(utc + JST_OFFSET);
  }

  /// 時分秒(とミリ秒)から `TimeOfDay` を作成する。
  static TimeOfDay fromHms(int h = 0, int m = 0, int s = 0, int ms = 0) {
    return TimeOfDay(std::chrono::hours(h) + std::chrono::minutes(m) +
                     std::chrono::seconds(s) + std::chrono::milliseconds(ms));
  }

  /// 深夜からの経過時間。
  decltype(m_timeSinceMidnight) timeSinceMidnight() const {
    return m_timeSinceMidnight;
  }

  /// 時の位を返す。
  int hour() const {
    return std::chrono::duration_cast<std::chrono::hours>(m_timeSinceMidnight)
               .count() %
           24;
  }

  /// 分の位を返す。
  int minute() const {
    return std::chrono::duration_cast<std::chrono::minutes>(m_timeSinceMidnight)
               .count() %
           60;
  }

  /// 秒の位を返す。
  int second() const {
    return std::chrono::duration_cast<std::chrono::seconds>(m_timeSinceMidnight)
               .count() %
           60;
  }

  /// ミリ秒の位を返す。
  int millisecond() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               m_timeSinceMidnight)
               .count() %
           1000;
  }

  /// 特定の時刻であるか、或いはそれを過ぎて一定時間以内であるかどうかを返す。
  ///
  /// `target` の時刻ちょうどであるか、それ以降 `futureDuration`
  /// 以内の時間であれば `true` を返す。
  ///
  /// `futureDuration` はその名の通り未来方向への範囲であり、 `target` よりも
  /// 前の時間について指定するものではない。
  template <typename Rep, typename Period>
  bool isAfter(TimeOfDay target,
               std::chrono::duration<Rep, Period> futureDuration) {
    auto diff = (timeSinceMidnight() + std::chrono::hours(24) -
                 target.timeSinceMidnight()) %
                std::chrono::hours(24);
    return (diff.count() >= 0 && diff <= futureDuration);
  }

  template <typename Rep, typename Period>
  TimeOfDay operator+(const std::chrono::duration<Rep, Period> &rhs) {
    return TimeOfDay(m_timeSinceMidnight + rhs);
  }

  template <typename Rep, typename Period>
  TimeOfDay &operator+=(const std::chrono::duration<Rep, Period> &rhs) {
    *this = *this + rhs;
    return *this;
  }

  template <typename Rep, typename Period>
  TimeOfDay operator-(const std::chrono::duration<Rep, Period> &rhs) {
    return TimeOfDay(m_timeSinceMidnight - rhs);
  }

  template <typename Rep, typename Period>
  TimeOfDay &operator-=(const std::chrono::duration<Rep, Period> &rhs) {
    *this = *this - rhs;
    return *this;
  }

  bool operator==(const TimeOfDay &rhs) const {
    return timeSinceMidnight() == rhs.timeSinceMidnight();
  }

  bool operator<(const TimeOfDay &rhs) {
    return timeSinceMidnight() < rhs.timeSinceMidnight();
  }

  bool operator<=(const TimeOfDay &rhs) {
    return timeSinceMidnight() <= rhs.timeSinceMidnight();
  }

  bool operator>(const TimeOfDay &rhs) {
    return timeSinceMidnight() > rhs.timeSinceMidnight();
  }

  bool operator>=(const TimeOfDay &rhs) {
    return timeSinceMidnight() >= rhs.timeSinceMidnight();
  }

private:
  template <typename Rep, typename Period>
  static std::chrono::duration<Rep, Period>
  durationAbs(const std::chrono::duration<Rep, Period> &d) {
    return d >= d.zero() ? d : -d;
  }

  template <typename Rep, typename Period>
  static std::chrono::duration<Rep, Period>
  modulo24h(const std::chrono::duration<Rep, Period> &d) {
    constexpr auto DAY = std::chrono::hours(24);
    if (d >= std::chrono::duration<Rep, Period>::zero()) {
      return d % DAY;
    } else {
      auto backward = durationAbs(d) % DAY;
      return (DAY - backward) % DAY;
    }
  }
};
} // namespace sugar
#endif
