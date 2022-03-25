#include "Sync.h"

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

void Sync::adjust_phase_kuramoto() {
  std::cout << "Adjusting phase based on kuramoto model" << std::endl;
  // insert code based on kuramoto model
  // https://github.com/owingit/fireflysync/blob/master/kuramato_better_sensor/kuramato_better_sensor.ino
  return;
}

void Sync::start() {
  std::cout << "Starting Sync" << std::endl;

  // here we would have the following algorithm:
  // capture and process image from realsense
  // if something is detected then feed to kuramoto model
  // change the led blink phase
  // repeat until an interrupt happens
  while (STATUS == Status::ON) {
    bool is_detected = camera_.is_flash_detected();  // non-blocking call
    if (is_detected == true) {
      adjust_phase_kuramoto();  // would calculate a new phase
      blink_.set_phase(1);      // sample input 1 sec phase
    }
  }
  std::cout << "Exiting Sync" << std::endl;
}

}  // namespace fireflybot