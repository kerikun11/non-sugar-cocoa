/**
 * @file hardware.cpp
 * @author Masashi Oyama (mabo168general@gmail.com)
 * @brief ハードウェア周りの関数
 * @version 0.1
 * @date 2018-12-01
 */

#include<string>
#include"hardware.h"
#include<WiFi.h>


void hardware::Hardware::drawClock()const{
    static byte omm = 99, oss = 99;//直前に描画を行った分・秒の値。これが現在時刻と異なっていたら描画する。分・秒が絶対に取りえない値で初期化することで、初めてのdrawClock()では描画処理が必ず発生するようにする
    static byte xcolon = 0, xsecs = 0;
    static unsigned int colour = 0;
    //現在時刻の取得
    uint8_t hh = 0, mm = 0, ss = 0;
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        hh = timeinfo.tm_hour;
        mm = timeinfo.tm_min;
        ss = timeinfo.tm_sec;
    }

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
        //ommの更新をし、分の値が変わらないうちは描画処理を行わないようにする
        omm = mm;
        //時間の描画、0でいいので2桁目を描画する
        if (hh < 10){
        xpos += M5.Lcd.drawChar('0', xpos, ypos,8); // Add hours leading zero for 24 hr clock
        }
        xpos += M5.Lcd.drawNumber(hh, xpos, ypos, 8); // Draw hours
        xcolon = xpos; //秒の描画において":"を描画するのに用いる
        //時間と分の間の":"の描画
        xpos += M5.Lcd.drawChar(':', xpos, ypos - 8, 8);//どうせ暗く塗りつぶされる
        //分の描画、0でいいので2桁目を描画する
        if (mm < 10){
            xpos += M5.Lcd.drawChar('0', xpos, ypos, 8); // Add minutes leading zero
        }
        xpos += M5.Lcd.drawNumber(mm, xpos, ypos, 8);  // Draw minutes
        xsecs = xpos; // Sae seconds 'x' position for later display updates
    }
    //毎秒の秒の描画
    if (oss != ss) {
        //ossの更新をし、秒の値が変わっていないうちは描画処理を行わないようにする
        oss = ss;
        xpos = xsecs;
        //":"の描画
        if (ss % 2) {
            //奇数秒は、":"を暗く描画
            M5.Lcd.setTextColor(0x39C4,TFT_BLACK); //文字色を暗くする
            M5.Lcd.drawChar(':', xcolon, ypos - 8, 8);     //時間と分の間の":"を暗く描画（分の描画で黄色く描画してても暗い":"で上書きしてしまう）
            xpos += M5.Lcd.drawChar(':', xsecs, ysecs, 6); //分と秒の間の":"を描画
            M5.Lcd.setTextColor(TFT_YELLOW, TFT_BLACK); //文字色を黄色に戻す
        } else {
            M5.Lcd.drawChar(':', xcolon, ypos - 8, 8);     //時間と分の間の":"を明るく描画
            xpos += M5.Lcd.drawChar(':', xsecs, ysecs, 6); //分と秒の間の":"を描画
        }
        //秒の描画、0でいいので2桁目を描画する
        if (ss < 10){
            xpos += M5.Lcd.drawChar('0', xpos, ysecs, 6); // Add leading zero
        }
        M5.Lcd.drawNumber(ss, xpos, ysecs, 6);          // Draw seconds
    }
    //*/
}

void hardware::Hardware::connectWiFi()const{
    //普通はSSIDとpassを引数に渡してやる
    //const char *SSID="ssid";
    //const char *passward="passward";
    //WiFi.begin(SSID,passward);//デバッグ環境によってSSIDとパスワードは変更してください
    WiFi.begin();//前回接続していたWiFiへの接続をさせる
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        M5.Lcd.print(".");
    }
    M5.Lcd.println(" WiFi CONNECTED");
}

void hardware::Hardware::disconnectWiFi()const{
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
}

void hardware::Hardware::initClock()const{
    //WiFi接続
    connectWiFi();
    //時刻合わせ
    const char *ntpServer = "ntp.jst.mfeed.ad.jp"; //日本のNTPサーバー選択
    const long gmtOffset_sec = 9 * 3600; // 9時間の時差を入れる
    const int daylightOffset_sec = 0;    //夏時間はないのでゼロ
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    M5.Lcd.println("config Time");
    struct tm timeinfo;
    while(true){
        //たまにgetLocalTime()に失敗するので、成功するのを確認するまでWiFiを切断しない。
        //configTime()を複数回する必要がない事は確認している。
        if(getLocalTime(&timeinfo)){
            M5.Lcd.println("set OK");
            break;
        }else{
            M5.Lcd.println("set NG");
        }
    }
    //WiFi切断、電力消費を抑えるために切断必須
    disconnectWiFi();
    M5.Lcd.println("WIFI DISCONNECTED");
}
