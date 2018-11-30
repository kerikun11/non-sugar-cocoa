#pragma once

// for M5.Btn*
#include <M5Stack.h>
// for xTaskCreate() etc.
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace hardware {

/// ボタンの種類
enum class Button { A, B, C };

}; // namespace hardware
