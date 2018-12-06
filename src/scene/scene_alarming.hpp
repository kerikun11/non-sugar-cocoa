/**
 * @file scene_alarming.hpp
 * @author Sekihara Takeshi (seki.maq.kan@gmail.com)
 * @brief 現在時刻を表示するシーン
 * @version 0.1
 * @date 2018-12-02
 */
#pragma once

#include "hardware/hardware.h"
#include "scene/event.hpp"
#include "scene/scene.hpp"

#include <cstdlib>

// コンパイルエラーを防ぐため， Arduino.h で定義されているマクロをundef
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#include <chrono>

namespace scene {

// アラーム設定時刻になった後，アラーム音が鳴り続けている最中のScene
class SceneAlarming : public Scene {
public:
  // コンストラクタ，必要なものがあれば受け取る仕様にする
  SceneAlarming(std::shared_ptr<hardware::Hardware> &m_hardware)
      : m_hardware(m_hardware) {}

  /// シーンがスタックのトップに来たとき呼ばれる。
  virtual EventResult activated() override {
    log_i("SceneAlarming activated()");

    // LCDのクリア
    M5.Lcd.clear();
    //文字色設定
    M5.Lcd.setTextColor(TFT_YELLOW, TFT_BLACK);
    // 描画
    drawShakingCount(max_count, true);

    //アラーム音の再生を開始
    m_hardware->speaker().play(hardware::SpeakerManager::Music::Alarm);
    tick_counter_for_beep = 0;

    // hardware側の振動回数の初期化(Countを使用するのは一人だと仮定．よそで勝手に操作されると困る)
    m_hardware->shaking().resetCount();
    m_hardware->shaking().startCount();

    // 絶起ツイートの時間を設定
    m_timelimit_to_stop = std::chrono::system_clock::now() + alarming_duration;

    return EventResultKind::Continue;
  }

  /// 定期的に (タイマーイベントごとに) 呼ばれる。
  virtual EventResult tick() override {

    //音のON/OFF処理
    tick_counter_for_beep += 1;
    if( static_cast<int>(tick_counter_for_beep / 10) % 2 == 0){
      m_hardware->speaker().stop();
    }else{
      m_hardware->speaker().play(hardware::SpeakerManager::Music::Alarm); 
    }

    auto now_count = m_hardware->shaking().getCount();

    if (now_count >= max_count) {
      stopAlarm();

      m_hardware->tweet().tweet(
          "はいプロ\n世界一起床が上手\n起床界のtourist\n布団時代の終"
          "焉を告げる者\n実質朝\n起床するために生まれてきた男");
      log_i("SceneAlarming kisyou_success Finish");
      return EventResultKind::Finish;
    }

    int remain_count = max_count - m_hardware->shaking().getCount();
    updateLcd(remain_count, remain_count);

    //アラーム制限時間処理
    if (std::chrono::system_clock::now() > m_timelimit_to_stop) {

      //起床失敗 tweet & alarm停止
      stopAlarm();

      m_hardware->tweet().tweet("絶起");
      log_i("SceneAlarming kisyou_failed Finish");
      return EventResultKind::Finish;
    }

    return EventResultKind::Continue;
  }

  /// ボタンが押されても何もしないので，ボタン関連overrideはしない

protected:
  std::shared_ptr<hardware::Hardware> m_hardware;

  // アラームを止めるために振らなければならない回数
  const int max_count = 5;
  //アラームが鳴り続ける最大時間．これを過ぎるとぜっきTweet
  const std::chrono::seconds alarming_duration{10};

  int tick_counter_for_beep = 0;

  std::chrono::system_clock::time_point m_timelimit_to_stop =
      std::chrono::system_clock::now();

private:
  void updateLcd(int remain_count, int sec, bool clean = false) const {
    // 中央のふる回数の表示
    drawShakingCount(remain_count, clean);
    // 右下の残り時間の表示
    DrawContactAfter(sec, clean);
  }

  void drawShakingCount(int remain_count, bool clean) const {
    static int prev_count = 0;
    if (!clean && remain_count == prev_count)
      return;
    prev_count = remain_count;
    //描画位置
    int xpos = 160;
    int ypos = 75 - 24; // Top left corner ot clock text, about half way down

    //描画処理
    // 文字は白で表示
    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Lcd.drawCentreString("Shake to Stop!", xpos, ypos, 4); // Draw hours
    ypos += 54;
    // ふる回数は黄色で表示
    M5.Lcd.setTextColor(TFT_YELLOW, TFT_BLACK);
    M5.Lcd.drawCentreString(String(remain_count, DEC), xpos, ypos, 8);
  }

  // 右下の描画をする
  void DrawContactAfter(int sec, bool clean) const {
    static int prev_count = 0;
    if (!clean && sec == prev_count)
      return;
    prev_count = sec;
    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    // 描画文字列str。文字列がずれる時の対策として先頭に半角スペースを数個追加する
    std::string str = " Automatically Send a Message After ";
    // sprintfで秒数を格納する
    char c[10];
    // str+=std::to_string(30);//< コンパイル通らないんやが
    sprintf(c, "%d", sec);
    str += c;
    str += " [s]";
    // 描画
    M5.Lcd.drawRightString(str.c_str(), 300, 220, 2);
  }

  void stopAlarm() {
    m_hardware->speaker().stop(); //音の停止

    //カウントの停止とリセット
    m_hardware->shaking().stopCount();
    m_hardware->shaking().resetCount();

    // スレッドローカルなアラーム状態キャッシュを更新
    hardware::Hardware::setAlarmEnabled(false);
  }
};

}; // namespace scene
