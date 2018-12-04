/**
 * @file scene_boot.hpp
 * @author Ryotaro Onuki (kerikun11+github@gmail.com)
 * @brief ブート画面
 * @date 2018-12-04
 */
#pragma once

#include <WiFi.h>

#include "../hardware/hardware.h"
#include "scene/event.hpp"
#include "scene/scene.hpp"
#include "scene/scene_clock.hpp"

namespace scene {

// アラーム設定時刻になった後，アラーム音が鳴り続けている最中のScene
class SceneBoot : public Scene {
public:
  // コンストラクタ，必要なものがあれば受け取る仕様にする
  SceneBoot(std::shared_ptr<hardware::Hardware> &m_hardware)
      : m_hardware(m_hardware) {}

  /// シーンがスタックのトップに来たとき呼ばれる。
  virtual EventResult activated() override {
    // LCDのクリア
    M5.Lcd.clear();
    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    // WiFi接続
    isConnected = false;
    WiFi.begin();
    M5.Lcd.drawCentreString("WiFi connecting...", 160, 120 - 12, 4);
    // SNTP の初期化
    const char *ntpServer = "ntp.jst.mfeed.ad.jp"; //日本のNTPサーバー選択
    const long gmtOffset_sec = 9 * 3600; // 9時間の時差を入れる
    const int daylightOffset_sec = 0;    //夏時間はないのでゼロ
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    return EventResultKind::Continue;
  }

  /// 定期的に (タイマーイベントごとに) 呼ばれる。
  virtual EventResult tick() override {
    // WiFi接続が完了したら表示を更新
    if (isConnected == false && WiFi.isConnected()) {
      isConnected = true;
      M5.Lcd.drawCentreString("Getting Time via NTP...", 160, 120 - 12, 4);
    }
    // 時間を取得し終わったら次のシーンへ
    if (time(NULL) > 30 * 365 * 24 * 60 * 60) {
      WiFi.mode(WIFI_OFF);
      return EventResult(EventResultKind::ReplaceScene,
                         static_cast<void *>(new SceneClock(m_hardware)));
    }
    return EventResultKind::Continue;
  }

  /// ボタン
  virtual EventResult buttonAPressed() override {
    // WiFi あきらめ
    return EventResult(EventResultKind::ReplaceScene,
                       static_cast<void *>(new SceneClock(m_hardware)));
  }

  /// ボタンC
  virtual EventResult buttonCPressed() override {
    // WiFi SmartConfig
    M5.Lcd.drawCentreString("WiFi SmartConfig Start!", 160, 120 - 12, 4);
    WiFi.mode(WIFI_AP_STA);
    WiFi.beginSmartConfig();
    return EventResultKind::Continue;
  }

protected:
  std::shared_ptr<hardware::Hardware> m_hardware;
  bool isConnected;
};

}; // namespace scene
