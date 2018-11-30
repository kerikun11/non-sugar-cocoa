/**
 * @file main.cpp
 * @author Ryotaro Onuki (kerikun11@gmail.com)
 * @brief アプリケーションの起点はこのファイルに書く
 * @date 2018-11-29
 */
#include <esp32-hal-log.h>

#include "hardware/hardware.h"

hardware::Hardware hw;

void setup() {
  // put your setup code here, to run once:
  log_i("Hello World!");
  hw.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
}
