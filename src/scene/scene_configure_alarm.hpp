/**
 * @file scene_set_clock.cpp
 * @author Masashi Oyama (mabo168general@gmail.com)
 * @brief 時刻合わせ場面のクラス
 * @version 0.1
 * @date 2018-12-01
 */

#pragma once

#include "../hardware/hardware.h"
#include "scene.hpp"

namespace scene {

/// Scene を継承する
class SceneConfigureAlarm : public Scene {
public:
  SceneConfigureAlarm() {}

  /// シーンがスタックのトップに来たとき呼ばれる。
  virtual EventResult activated() override {
    log_i("SceneSetAlarm activated()");
    // LCDのクリア
    M5.Lcd.clear();
    //実際には、すでに記録されている時刻通りに初期化する
    m_hour = 0;
    m_min = 0;
    m_sec = 0;
    //その他の初期化
    m_process = 0;
    // ごみを消去
    M5.Lcd.clear();
    return EventResultKind::Continue;
  }

  /// 定期的に (タイマーイベントごとに) 呼ばれる。
  virtual EventResult tick() override {
    // TODO: 時刻が変わったときだけ更新
    updateDisplay();
    return EventResultKind::Continue;
  }

  virtual EventResult buttonAPressed() override {
    proceedProcess();
    updateDisplay();
    return EventResultKind::Continue;
  }
  virtual EventResult buttonBPressed() override {
    increment();
    updateDisplay();
    return EventResultKind::Continue;
  }
  virtual EventResult buttonCPressed() override {
    // TODO: アラーム設定完了の処理
    return EventResultKind::Finish;
  }

private:
  uint8_t m_hour, m_min, m_sec; //時刻
  int m_process; //編集する場所。この書き方は汚い。(0:時間 1:分 2:秒)

  void proceedProcess() {
    //編集箇所を次に進める
    m_process = (m_process + 1) % 3;
    //ディスプレイの更新
    updateDisplay();
  }

  void increment() {
    //数値をインクリメント
    switch (m_process) {
    case (0):
      m_hour = (m_hour + 1) % 24;
      break;
    case (1):
      m_min = (m_min + 1) % 60;
      break;
    case (2):
      m_sec = (m_sec + 1) % 60;
      break;
    }
  }
  void decrement() {
    //数値をデクリメント
    switch (m_process) {
    case (0):
      m_hour = (m_hour + 23) % 24;
      break;
    case (1):
      m_min = (m_min + 59) % 60;
      break;
    case (2):
      m_sec = (m_sec + 59) % 60;
      break;
    }
  }

  // 画面を更新する
  void updateDisplay()const{
    // ボタン上のUIの描画
    const int32_t rectY=212;
    const int32_t width=20;
    const int32_t height=6;
    const int32_t leftX=56;
    const int32_t rightX=245;
    // 一番左は"-"を描画（長方形の描画を用いる） 
    M5.Lcd.setTextColor(TFT_PINK, TFT_BLACK);
    //M5.Lcd.drawChar('-',53,200,6);//< これは正しく表示されるが、なんとなく見づらい
    M5.Lcd.fillRect(leftX,rectY,width,height,TFT_PINK);
    // 中央は"->"を右方向にアニメーションして描画
    M5.Lcd.setTextColor(TFT_PINK, TFT_BLACK);
    M5.Lcd.drawString("NEXT",130,205,4);
    // 一番右は"+"を描画（長方形の描画を用いる）
    M5.Lcd.setTextColor(TFT_PINK, TFT_BLACK);
    //M5.Lcd.drawChar('+',250,200,6);//< サイズ6だと+が表示されない、意味わからん……
    M5.Lcd.fillRect(rightX,rectY,width,height,TFT_PINK);
    M5.Lcd.fillRect(rightX+(width-height)/2,rectY+(height-width)/2,height,width,TFT_PINK);

    // 今指し示しているものがわかるように、数字の上下に三角形の描画
    static int beforeProcess=99;
    if(beforeProcess!=m_process){
      // 前フレームで描画していた三角形の描画削除
      DrawProcessTriangle(beforeProcess,TFT_BLACK);
      // 今フレームで描画する三角形の描画
      DrawProcessTriangle(m_process,TFT_YELLOW);
      beforeProcess=m_process;
    }

    // 時刻部分の更新
    updateDisplayClock();
  }

  // 現在設定している部分の描画
  void DrawProcessTriangle(int process,uint16_t color)const{
    // 描画位置の決定
    int x=-100,y1=-100,y2=-100;
    const int v1x=5,v1y=7;
    const int v2x=10,v2y=0;
    switch(process){
    case (0):
      x=51;
      y1=75;
      y2=175;
      break;
    case (1):
      x=187;
      y1=75;
      y2=175;
      break;
    case (2):
      x=285;
      y1=100;
      y2=155;
      break;
    }
    //描画
    M5.Lcd.fillTriangle(x,y1,x+v1x,y1-v1y,x+v2x,y1-v2y,color);
    M5.Lcd.fillTriangle(x,y2,x+v1x,y2+v1y,x+v2x,y2+v2y,color);
  }

  // 画面のうち時刻部分を更新する
  void updateDisplayClock() const {
    //描画準備
    //文字色設定
    M5.Lcd.setTextColor(TFT_YELLOW, TFT_BLACK);
    //描画位置
    int xpos = 0;
    int ypos = 85; // Top left corner ot clock text, about half way down
    int ysecs = ypos + 24;

    //描画処理
    //時間の描画、0でいいので2桁目を描画する
    xpos += drawNumber2Char(m_hour, xpos, ypos, 8);
    //時間と分の間の":"の描画
    xpos += M5.Lcd.drawChar(':', xpos, ypos - 8, 8);
    //分の描画、0でいいので2桁目を描画する
    xpos += drawNumber2Char(m_min, xpos, ypos, 8);
    //分と秒の間の":"の描画
    xpos += M5.Lcd.drawChar(':', xpos, ysecs, 6);
    //秒の描画、0でいいので2桁目を描画する
    xpos += drawNumber2Char(m_sec, xpos, ysecs, 6);
  }
  int drawNumber2Char(uint8_t pal, int x, int y, int font) const {
    // 2文字で数字の描画(1桁は先頭に0をつけて描画する)
    int dx = 0; //文字列描画の横幅
    if (pal < 10) {
      dx += M5.Lcd.drawChar('0', x, y, font);
    }
    dx += M5.Lcd.drawNumber(pal, x + dx, y, font);
    return dx;
  }
};

}; // namespace scene
