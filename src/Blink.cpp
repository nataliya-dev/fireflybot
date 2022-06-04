#include "Blink.h"

#include <iostream>
#include <thread>
namespace fireflybot {

bool Blink::initialize() {
  std::cout << "Initializing Blink" << std::endl;

  if (is_sim_) {
    std::cout << "Blink simulation mode." << std::endl;
    return true;
  }

  int ret = wiringPiSetup();
  std::cout << "setup return val: " << ret << std::endl;
  pinMode(LED, OUTPUT);

  return true;
}

void Blink::turn_led_on() {
  std::cout << "\nLED on" << std::endl;
  is_led_on_ = true;

  if (is_sim_) {
    return;
  }

  digitalWrite(LED, HIGH);
  return;
}

void Blink::turn_led_off() {
  std::cout << "\nLED off" << std::endl;
  is_led_on_ = false;

  if (is_sim_) {
    return;
  }

  digitalWrite(LED, LOW);
  return;
}

void Blink::test_burst_blink() {
  while (true) {
    burst_blink();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}

void Blink::test_phase_blink() {
  while (true) {
    phase_blink();
  }
}

void Blink::burst_blink() {
  turn_led_on();
  led_trigger_tm_ = std::chrono::high_resolution_clock::now();
  led_trigger_tm_ += std::chrono::milliseconds(HARDWARE_PROCESSING_MS_);
  std::this_thread::sleep_for(std::chrono::milliseconds(LED_DURATION_MS));
  turn_led_off();
}

long int Blink::get_init_sync_period() { return INITIAL_SYNC_PERIOD_MS; }
void Blink::set_init_sync_period(long int period) {  INITIAL_SYNC_PERIOD_MS = period; }

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

bool Blink::get_state() { return flashing_; }
void Blink::set_state(bool state) { flashing_ = state; }

long int Blink::get_nf() { return n_f_; }
void Blink::set_nf(long int num_flash) { n_f_ = num_flash; }

long int Blink::get_interburst() { return t_b_; }
void Blink::set_interburst(long int interburst) { t_b_ = interburst; }

void Blink::calc_phase() {
  auto now_tm = std::chrono::high_resolution_clock::now();
  long int elapsed_time_ms =
      std::chrono::duration<double, std::milli>(now_tm - led_trigger_tm_)
          .count();
  // std::cout << "elapsed_time_ms: " << elapsed_time_ms << std::endl;
  // if (elapsed_time_ms < 0) {
  //   phase_ = 0;
  // }
  phase_ = elapsed_time_ms;
}

long int Blink::get_phase() {
  calc_phase();
  return phase_;
}

bool Blink::phase_blink() {
  calc_phase();
  bool do_blink = false;
  if (!is_led_on_ && phase_ > period_) {
    turn_led_on();
    led_trigger_tm_ = std::chrono::high_resolution_clock::now();
    led_trigger_tm_ += std::chrono::milliseconds(HARDWARE_PROCESSING_MS_);
    do_blink = true;
    std::cout << "phase_: " << phase_ << std::endl;
    std::cout << "period_: " << period_ << std::endl;

  } else if (is_led_on_ && phase_ > LED_DURATION_MS) {
    turn_led_off();
    std::cout << "phase_: " << phase_ << std::endl;
    std::cout << "period_: " << period_ << std::endl;
  }
  return do_blink;
}

void Blink::set_sim_mode(bool is_sim) { is_sim_ = is_sim; }

}  // namespace fireflybot
