#include "Blink.h"

#include <iostream>
namespace fireflybot {

bool Blink::initialize() {
  std::cout << "Initializing Blink" << std::endl;
  // insert raspberry pi LED initialization here
  return true;
}

void Blink::turn_led_on() {
  std::cout << "LED on" << std::endl;
  is_led_on_ = true;
  // insert code that turns on the led
  return;
}

void Blink::turn_led_off() {
  std::cout << "LED off" << std::endl;
  is_led_on_ = false;
  // insert code that turns off the led
  return;
}

long int Blink::get_init_sync_period() { return INITIAL_SYNC_PERIOD_MS; }

std::chrono::time_point<std::chrono::high_resolution_clock>
Blink::get_led_trigger_tm() {
  return led_trigger_tm_;
}

void Blink::set_led_trigger_tm(
    std::chrono::time_point<std::chrono::high_resolution_clock> tm) {
  led_trigger_tm_ = tm;
}

long int Blink::get_period() { return period_; }
void Blink::set_period(long int period) { period_ = period; }

void Blink::calc_phase() {
  auto now_tm = std::chrono::high_resolution_clock::now();
  long int elapsed_time_ms =
      std::chrono::duration<double, std::milli>(now_tm - led_trigger_tm_)
          .count();
  if (elapsed_time_ms > 0) {
    phase_ = 0;
  }
  phase_ = elapsed_time_ms;
}

long int Blink::get_phase() {
  calc_phase();
  return phase_;
}

void Blink::blink() {
  calc_phase();
  if (!is_led_on_ && phase_ > period_) {
    turn_led_on();
    led_trigger_tm_ = std::chrono::high_resolution_clock::now();
  }

  if (is_led_on_ && phase_ > LED_DURATION_MS) {
    turn_led_off();
  }
}

}  // namespace fireflybot