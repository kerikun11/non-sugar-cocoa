#include "shaking_manager.hpp"
#include <M5Stack.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace hardware {

// From Sample Program
void ShakingManager::updateMeasurement() {
  static bool initialized = false;
  if (!initialized) {
    initialized = true;
    IMU.calibrateMPU9250(IMU.gyroBias, IMU.accelBias);
    IMU.initMPU9250();
    IMU.initAK8963(IMU.magCalibration);
  }
  // If intPin goes high, all data registers have new data
  // On interrupt, check if data ready interrupt
  if (IMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01) {
    IMU.readAccelData(IMU.accelCount); // Read the x/y/z adc values
    IMU.getAres();

    // Now we'll calculate the accleration value into actual g's
    // This depends on scale being set
    IMU.ax = (float)IMU.accelCount[0] * IMU.aRes; // - accelBias[0];
    IMU.ay = (float)IMU.accelCount[1] * IMU.aRes; // - accelBias[1];
    IMU.az = (float)IMU.accelCount[2] * IMU.aRes; // - accelBias[2];

    IMU.readGyroData(IMU.gyroCount); // Read the x/y/z adc values
    IMU.getGres();

    // Calculate the gyro value into actual degrees per second
    // This depends on scale being set
    IMU.gx = (float)IMU.gyroCount[0] * IMU.gRes;
    IMU.gy = (float)IMU.gyroCount[1] * IMU.gRes;
    IMU.gz = (float)IMU.gyroCount[2] * IMU.gRes;

    IMU.readMagData(IMU.magCount); // Read the x/y/z adc values
    IMU.getMres();
    // User environmental x-axis correction in milliGauss, should be
    // automatically calculated
    IMU.magbias[0] = +470.;
    // User environmental x-axis correction in milliGauss TODO axis??
    IMU.magbias[1] = +120.;
    // User environmental x-axis correction in milliGauss
    IMU.magbias[2] = +125.;

    // Calculate the magnetometer values in milliGauss
    // Include factory calibration per data sheet and user environmental
    // corrections
    // Get actual magnetometer value, this depends on scale being set
    IMU.mx = (float)IMU.magCount[0] * IMU.mRes * IMU.magCalibration[0] -
             IMU.magbias[0];
    IMU.my = (float)IMU.magCount[1] * IMU.mRes * IMU.magCalibration[1] -
             IMU.magbias[1];
    IMU.mz = (float)IMU.magCount[2] * IMU.mRes * IMU.magCalibration[2] -
             IMU.magbias[2];
  }
}

void ShakingManager::updateCount() {

  //とりあえずx方向の角速度のみを使って検知
  auto swing_angle_velocity_x = IMU.ax;
  auto swing_angle_velocity_y = IMU.ay;
  auto swing_angle_velocity_z = IMU.az;

  switch (shaking_state) {

  case ShakingState::Counting:

    dir_x_state = dirStateChange(swing_angle_velocity_x, dir_x_state);
    dir_y_state = dirStateChange(swing_angle_velocity_y, dir_y_state);
    dir_z_state = dirStateChange(swing_angle_velocity_z, dir_z_state);

    count = std::max(count, dir_x_state.count);
    count = std::max(count, dir_y_state.count);
    count = std::max(count, dir_z_state.count);

    dir_x_state.count = count;
    dir_y_state.count = count;
    dir_z_state.count = count;

    break;

  case ShakingState::Stop:
    break;
  }
}

ShakingManager::OneDirectionState ShakingManager::dirStateChange(float swing_axis,
                                                 OneDirectionState dir_state) {
  OneDirectionState next_state = dir_state;
  const float threshold_swing_angle_axis = 1.6;
  switch (dir_state.state) {

  case OneDirection::UpperSwing:
    if (swing_axis > threshold_swing_angle_axis) {
      next_state.count++;
      log_d("up");
      next_state.state = OneDirection::LowerSwing;
    }
    break;

  case OneDirection::LowerSwing:
    if (swing_axis < -threshold_swing_angle_axis) {
      log_d("down");
      next_state.state = OneDirection::UpperSwing;
    }
    break;
  }

  return next_state;
}

}; // namespace hardware
