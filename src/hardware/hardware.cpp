#include "hardware.h"

namespace hardware {
thread_local bool Hardware::m_alarmIsEnabled{false};
thread_local sugar::TimeOfDay Hardware::m_alarmTimeCache{sugar::TimeOfDay{}};

// Workaround for GCC-5 bug
// <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66971>.
void Hardware::setAlarmEnabled(bool v) { m_alarmIsEnabled = v; }
// Workaround for GCC-5 bug
// <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66971>.
bool Hardware::isAlarmEnabled() { return m_alarmIsEnabled; }
// Workaround for GCC-5 bug
// <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66971>.
void Hardware::setAlarmTimeCache(sugar::TimeOfDay v) { m_alarmTimeCache = v; }
// Workaround for GCC-5 bug
// <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66971>.
const sugar::TimeOfDay &Hardware::alarmTimeCache() { return m_alarmTimeCache; }
} // namespace hardware
