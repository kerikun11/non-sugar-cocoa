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

void hardware::Hardware::connectWiFi()const{
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
