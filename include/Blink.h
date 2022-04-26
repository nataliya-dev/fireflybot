#ifndef FIREFLYBOT_BLINK_H
#define FIREFLYBOT_BLINK_H
#include <wiringPi.h>

#include <chrono>

namespace fireflybot {
class Blink {
 public:
  Blink(){};
  ~Blink(){};

  bool initialize();

  void phase_blink();
  void burst_blink();

  long int get_phase();
  long int get_init_sync_period();

  std::chrono::time_point<std::chrono::high_resolution_clock>
  get_led_trigger_tm();
  void set_led_trigger_tm(
      std::chrono::time_point<std::chrono::high_resolution_clock> tm);

  long int get_period();
  void set_period(long int period);

  void test_burst_blink();
  void test_phase_blink();

  void set_sim_mode(bool is_sim);

 private:
  const int LED = 29;
  bool is_sim_ = false;

  const long int INITIAL_SYNC_PERIOD_MS = 1400;
  const long int LED_DURATION_MS = 200;
  const long int HARDWARE_PROCESSING_MS_ = 30;
  std::chrono::time_point<std::chrono::high_resolution_clock> led_trigger_tm_ =
      std::chrono::high_resolution_clock::now();

  long int phase_ = 0;
  long int period_ = INITIAL_SYNC_PERIOD_MS;

  bool is_led_on_ = false;

  void turn_led_on();
  void turn_led_off();
  void calc_phase();
};
}  // namespace fireflybot

#endif /* FIREFLYBOT_BLINK_H */
