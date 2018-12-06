#pragma once
// コンパイルエラーを防ぐため， Arduino.h で定義されているマクロをundef
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#include <atomic>

#include <utility/MPU9250.h>

namespace hardware {

class ShakingManager {

public:
  //カウント値リセット
  void resetCount() {
    count = 0;
    dir_x_state.count = 0;
    dir_y_state.count = 0;
    dir_z_state.count = 0;
  }
  void stopCount() { shaking_state = ShakingState::Stop; } //カウント再開/開始
  void startCount() {
    shaking_state = ShakingState::Counting;
  }                                 //カウント一次停止
  int getCount() { return count; }; //現在のカウント数

  // イベント待機を開始する
  void begin() {
    // FreeRTOS により task() をバックグラウンドで実行
    const uint16_t stackSize = 4096;
    UBaseType_t uxPriority = 10;
    xTaskCreate(
        [](void *this_obj) { static_cast<ShakingManager *>(this_obj)->task(); },
        "ShakingManager", stackSize, this, uxPriority, NULL);
  }

protected:
  enum class ShakingState { Counting, Stop };

  // X,Y,Z三方向の振動状態
  enum class OneDirection { UpperSwing, LowerSwing };
  struct OneDirectionState {
    int count = 0;
    OneDirection state = OneDirection::UpperSwing;
  };

  MPU9250 IMU; // 9 axis Sensor

  std::atomic_int count{0};                        //現在のカウント数
  const int sampling_period = 100;                 //[ms]
  ShakingState shaking_state = ShakingState::Stop; //カウント計測の状態
  OneDirectionState dir_x_state;
  OneDirectionState dir_y_state;
  OneDirectionState dir_z_state;

  void updateMeasurement(); // IMU値の更新
  void updateCount();       //振った回数の更新
  OneDirectionState dirStateChange(float swing_axis,
                                   OneDirectionState dir_state);

  // FreeRTOS によって実行される関数
  void task() {
    // handleEvent の定期実行
    portTickType xLastWakeTime = xTaskGetTickCount();
    while (1) {
      updateMeasurement();
      updateCount();
      vTaskDelayUntil(&xLastWakeTime, sampling_period / portTICK_RATE_MS);
    }
    vTaskDelete(NULL);
  }
};

} // namespace hardware
