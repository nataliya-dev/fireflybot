#include "Blink.h"

#include <iostream>
namespace fireflybot {

bool Blink::initialize() {
  std::cout << "Initializing Blink" << std::endl;
  // insert raspberry pi LED initialization here
  return true;
}

void Blink::set_phase(double phase) {
  std::cout << "setting phase: " << phase << std::endl;
  phase_ = phase;
}

void Blink::led_on() {
  std::cout << "LED on" << std::endl;
  // insert code that turns on the led
  return;
}

void Blink::led_off() {
  std::cout << "LED off" << std::endl;
  // insert code that turns off the led
  return;
}

}  // namespace fireflybot