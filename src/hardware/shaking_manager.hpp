#pragma once
#include <utility/MPU9250.h>

namespace hardware {

class ShakingManager {

public:
  void resetCount() { count = 0; } //カウント値リセット
  void stopCount() { shaking_state = ShakingState::Stop; } //カウント再開/開始
  void startCount() {
    shaking_state = ShakingState::CountingUpperSwing;
  }                                 //カウント一次停止
  int getCount() { return count; }; //現在のカウント数

  // イベント待機を開始する
  void begin() {
    // FreeRTOS により task() をバックグラウンドで実行
    const uint16_t stackSize = 4096;
    UBaseType_t uxPriority = 1;
    xTaskCreate(
        [](void *this_obj) { static_cast<ShakingManager *>(this_obj)->task(); },
        "ShakingManager", stackSize, this, uxPriority, NULL);
  }

protected:
  enum class ShakingState { CountingUpperSwing, CountingLowerSwing, Stop };

  MPU9250 IMU; // 9 axis Sensor

  const float threshold_swing_angle_axis =
      200;       // 1カウントと見なす，角速度の大きさ(正の値)
  int count = 0; //現在のカウント数
  const int sampling_period = 100;                 //[ms]
  ShakingState shaking_state = ShakingState::Stop; //カウント計測の状態

  void updateMeasurement(); // IMU値の更新
  void updateCount();       //振った回数の更新

  // FreeRTOS によって実行される関数
  void task() {
    // handleEvent の定期実行
    portTickType xLastWakeTime = xTaskGetTickCount();
    while (1) {
      vTaskDelayUntil(&xLastWakeTime, sampling_period / portTICK_RATE_MS);

      updateMeasurement();
      updateCount();
    }
    vTaskDelete(NULL);
  }
};

} // namespace hardware
