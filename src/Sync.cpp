#include "Sync.h"

#include <chrono>
#include <iostream>

namespace fireflybot {

Status Sync::STATUS = Status::ON;

Sync::Sync() {}

bool Sync::initialize() {
  std::cout << "Initializing Sync" << std::endl;

  if (camera_.initialize() == false) {
    return false;
  }

  if (blink_.initialize() == false) {
    return false;
  }

  return true;
}

void Sync::adjust_period_kuramoto() {
  std::cout << "Adjusting phase based on kuramoto model" << std::endl;
  // https://github.com/owingit/fireflysync/blob/master/kuramato_better_sensor/kuramato_better_sensor.ino

  auto detected_tm = std::chrono::high_resolution_clock::now();
  auto led_trigger_tm = blink_.get_led_trigger_tm();
  long int elapsed_time_ms =
      std::chrono::duration<double, std::milli>(detected_tm - led_trigger_tm)
          .count();
  long int adjust_ms = 0;
  long int period = blink_.get_period();
  if (elapsed_time_ms > period / 2) {
    adjust_ms = elapsed_time_ms;
  } else {
    adjust_ms = elapsed_time_ms - period;
  }

  adjust_ms /= PHASE_SHIFT_FACTOR;
  led_trigger_tm += std::chrono::milliseconds(adjust_ms);
  blink_.set_led_trigger_tm(led_trigger_tm);

  period = adjust_ms / PERIOD_CHANGE_FACTOR;
  blink_.set_period(period);

  return;
}

void Sync::start() {
  std::cout << "Starting Sync" << std::endl;

  while (STATUS == Status::ON) {
    bool is_detected = camera_.is_flash_detected();
    if (is_detected == true) {
      adjust_period_kuramoto();
    }
    blink_.blink();
  }
  std::cout << "Exiting Sync" << std::endl;
}

}  // namespace fireflybot