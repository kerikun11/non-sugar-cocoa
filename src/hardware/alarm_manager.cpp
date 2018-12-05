#include "alarm_manager.hpp"
#include "hardware.h"

namespace hardware {

bool AlarmTimeSetter::setAlarmTime(const sugar::TimeOfDay &time) {
  Hardware::setAlarmEnabled(true);
  Hardware::setAlarmTimeCache(time);
  auto obj = std::make_unique<sugar::TimeOfDay>(time);
  sugar::TimeOfDay *ptr = obj.release();
  return xQueueSendToBack(m_queue, &ptr, 0) == pdTRUE;
}
} // namespace hardware
