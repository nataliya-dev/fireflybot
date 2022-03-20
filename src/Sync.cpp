#include "Sync.h"

#include <iostream>

namespace fireflybot {
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
  std::size_t num = 5;
  for (std::size_t i = 0; i < num; i++) {
    camera_.wait_for_flash();  // blocking call? tbd
    adjust_phase_kuramoto();   // would calculate a new phase
    blink_.set_phase(1);       // sample input 1 sec phase
  }
}

}  // namespace fireflybot