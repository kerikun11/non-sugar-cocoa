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
#include "scene_set_clock.hpp"

namespace scene {

// アラーム設定時刻になった後，アラーム音が鳴り続けている最中のScene
class SceneAlerming : public Scene {
public:
  // コンストラクタ，必要なものがあれば受け取る仕様にする
  SceneAlerming(std::shared_ptr<hardware::Hardware> &m_hardware)
      : m_hardware(m_hardware) {}

  /// 定期的に (タイマーイベントごとに) 呼ばれる。
  virtual EventResult tick() override {

    return EventResultKind::Continue;
  }

  /// シーンがスタックのトップに来たとき呼ばれる。
  virtual EventResult activated() override {
    // ごみを消去
    // M5.Lcd.fillScreen(0);
    log_i("SceneAlerming activated()");

	//アラーム音の再生を開始
	m_hardware->play(Hardware::SpeakerManager::Music::Alerm );

    return EventResultKind::Continue;
  }

  /// ボタンが押されても何もしないので，ボタン関連overrideはしない

protected:
  std::shared_ptr<hardware::Hardware> m_hardware;


    //描画準備
    //文字色設定
    M5.Lcd.setTextColor(TFT_YELLOW, TFT_BLACK);
    //描画位置
    int xpos = 0;
    int ypos = 85; // Top left corner ot clock text, about half way down
    int ysecs = ypos + 24;

    //描画処理
    //毎分の時間・分の描画（分が変更していれば、時間は変わっていなくても時間を描画しなおす）
    if (omm != mm) {
      // ommの更新をし、分の値が変わらないうちは描画処理を行わないようにする
      omm = mm;
      //時間の描画、0でいいので2桁目を描画する
      if (hh < 10) {
        xpos += M5.Lcd.drawChar('0', xpos, ypos,
                                8); // Add hours leading zero for 24 hr clock
      }
      xpos += M5.Lcd.drawNumber(hh, xpos, ypos, 8); // Draw hours
      xcolon = xpos; //秒の描画において":"を描画するのに用いる
      //時間と分の間の":"の描画
      xpos +=
          M5.Lcd.drawChar(':', xpos, ypos - 8, 8); //どうせ暗く塗りつぶされる
      //分の描画、0でいいので2桁目を描画する
      if (mm < 10) {
        xpos += M5.Lcd.drawChar('0', xpos, ypos, 8); // Add minutes leading zero
      }
      xpos += M5.Lcd.drawNumber(mm, xpos, ypos, 8); // Draw minutes
      xsecs = xpos; // Sae seconds 'x' position for later display updates
    }
    //毎秒の秒の描画
    if (oss != ss) {
      // ossの更新をし、秒の値が変わっていないうちは描画処理を行わないようにする
      oss = ss;
      xpos = xsecs;
      //":"の描画
      if (ss % 2) {
        //奇数秒は、":"を暗く描画
        M5.Lcd.setTextColor(0x39C4, TFT_BLACK); //文字色を暗くする
        M5.Lcd.drawChar(
            ':', xcolon, ypos - 8,
            8); //時間と分の間の":"を暗く描画（分の描画で黄色く描画してても暗い":"で上書きしてしまう）
        xpos += M5.Lcd.drawChar(':', xsecs, ysecs, 6); //分と秒の間の":"を描画
        M5.Lcd.setTextColor(TFT_YELLOW, TFT_BLACK); //文字色を黄色に戻す
      } else {
        M5.Lcd.drawChar(':', xcolon, ypos - 8,
                        8); //時間と分の間の":"を明るく描画
        xpos += M5.Lcd.drawChar(':', xsecs, ysecs, 6); //分と秒の間の":"を描画
      }
      //秒の描画、0でいいので2桁目を描画する
      if (ss < 10) {
        xpos += M5.Lcd.drawChar('0', xpos, ysecs, 6); // Add leading zero
      }
      M5.Lcd.drawNumber(ss, xpos, ysecs, 6); // Draw seconds
    }
  }
};

}; // namespace scene
