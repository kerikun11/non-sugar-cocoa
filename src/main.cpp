/**
 * @file main.cpp
 * @author Ryotaro Onuki (kerikun11@gmail.com)
 * @brief アプリケーションの起点はこのファイルに書く
 * @date 2018-11-29
 */
#include <esp32-hal-log.h>

#include "hardware/hardware.h"
#include "scene/scene_manager.hpp"

std::shared_ptr<hardware::Hardware> hw;
scene::SceneManager scene_manager;

void setup() {
  // put your setup code here, to run once:
  log_i("Hello World!");
  hw = std::make_shared<hardware::Hardware>();
  hw->begin();
  scene_manager.initialize(hw);
}

void loop() {
  // put your main code here, to run repeatedly:
}
