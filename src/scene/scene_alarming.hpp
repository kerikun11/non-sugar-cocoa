/**
 * @file scene_alerming.hpp
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

namespace scene {

// アラーム設定時刻になった後，アラーム音が鳴り続けている最中のScene
class SceneAlarming : public Scene {
public:
  // コンストラクタ，必要なものがあれば受け取る仕様にする
  SceneAlarming(std::shared_ptr<hardware::Hardware> &m_hardware)
      : m_hardware(m_hardware) {}

  /// シーンがスタックのトップに来たとき呼ばれる。
  virtual EventResult activated() override {
    log_i("SceneAlerming activated()");

    // LCDのクリア
    M5.Lcd.clear();
    //文字色設定
    M5.Lcd.setTextColor(TFT_YELLOW, TFT_BLACK);

    //アラーム音の再生を開始
    m_hardware->speaker().play(hardware::SpeakerManager::Music::Alarm);

    // hardware側の振動回数の初期化(Countを使用するのは一人だと仮定．よそで勝手に操作されると困る)
    m_hardware->shaking().resetCount();
    m_hardware->shaking().startCount();

    return EventResultKind::Continue;
  }

  /// 定期的に (タイマーイベントごとに) 呼ばれる。
  virtual EventResult tick() override {

    auto now_count = m_hardware->shaking().getCount();

    if (now_count >= max_count) {
      m_hardware->speaker().stop(); //音の停止

      //カウントの停止とリセット
      m_hardware->shaking().stopCount();
      m_hardware->shaking().resetCount();

      log_i("SceneAlerming Finish");
      return EventResultKind::Finish;
    }

    int remain_count = max_count - m_hardware->shaking().getCount();
    updateLcd(remain_count);

    return EventResultKind::Continue;
  }

  /// ボタンが押されても何もしないので，ボタン関連overrideはしない

protected:
  std::shared_ptr<hardware::Hardware> m_hardware;

  const int max_count = 5; // 100回振ると，アラーム停止

private:
  void updateLcd(int remain_count) const {
    static int prev_count = 0;
    if (remain_count == prev_count)
      return;
    prev_count = remain_count;
    //描画位置
    int xpos = 160;
    int ypos = 75 - 24; // Top left corner ot clock text, about half way down

    //描画処理
    // 文字は白で表示
    M5.Lcd.setTextColor(TFT_WHITE,TFT_BLACK);
    M5.Lcd.drawCentreString("Shake to Stop!", xpos, ypos, 4); // Draw hours
    ypos += 54;
    // ふる回数は黄色で表示
    M5.Lcd.setTextColor(TFT_YELLOW,TFT_BLACK);
    M5.Lcd.drawCentreString(String(remain_count, DEC), xpos, ypos, 8);
    // 右下の残り時間は白で表示
    M5.Lcd.setTextColor(TFT_WHITE,TFT_BLACK);
    std::string str="    Contact after ";//< 文字列がずれる時の対策として先頭に半角スペースを数個追加する
    {
      char c[10];
      static int sec=999;
      //str+=std::to_string(30);//< コンパイル通らないんやが
      sprintf(c,"%d",sec);
      str+=c;
      sec--;
    }
    str+=" seconds...";
    M5.Lcd.drawRightString(str.c_str(),300,220,2);
  }
};

}; // namespace scene
