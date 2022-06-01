#ifndef FIREFLYBOT_BLINK_H
#define FIREFLYBOT_BLINK_H
#include <wiringPi.h>

#include <chrono>

namespace fireflybot {
class Blink {
 public:
  Blink(){};
  ~Blink(){};

  /**
   * Initialize the operational parameters of the interface to the LED.
   *
   * @return bool value is True if initialization is a success, False on
   * initialization failure.
   */
  bool initialize();

  /**
   * A call which only flashes the LED only if a certain amount of time has
   * passed. Used for the Kuramoto model.
   *
   * @return Whether or not the blinker turned on the light.
   */
  bool phase_blink();

  /**
   * A blocking call which turns the LED on, waits, and then turns it off.
   * Used for the integrate and fire model type synchronization.
   */
  void burst_blink();

  /**
   * Determing how much time has passed since the last LED light.
   *
   * @return Duration in ms.
   */
  long int get_phase();

  /**
   * Get the default setting for duration between LED flashes.
   *
   * @return Duration in ms.
   */
  long int get_init_sync_period();

  /**
   * Getter for time at which the LED needs to flash.
   *
   * @return Timepoint chrono.
   */
  std::chrono::time_point<std::chrono::high_resolution_clock>
  get_led_trigger_tm();

  /**
   * Setter for the time at which the LED needs to flash.
   *
   * @param[in] tm Timepoint chrono.
   */
  void set_led_trigger_tm(
      std::chrono::time_point<std::chrono::high_resolution_clock> tm);

  /**
   * Getter for the period at which the fireflybot should flash its LED.
   *
   * @return Duration ms.
   */
  long int get_period();

  /**
   * Setter for the period at which the fireflybot should flash its LED.
   *
   * @param[in] Duration ms.
   */
  void set_period(long int period);

  /**
   * Setter for the flashing state
   **/
  void set_state(bool state);
  /**
   * Getter for the flashing state
   **/
  bool get_state();
  
  /**
   * Setter for the flash count
   **/
  void set_nf(long int num_flash);
  /**
   * Getter for the flash count
   **/
  long int get_nf();
  /**
   * Setter for the starting period at which the fireflybot should flash its LED.
   *
   * @param[in] Duration ms.
   */
  void set_init_sync_period(long int period);

  /**
   * Open loop test for the burst_blink function.
   */
  void test_burst_blink();

  /**
   * Open loop test for the phase_blink function.
   */
  void test_phase_blink();

   /**
   * Turn the LED off.
   */
  void turn_led_on();

  /**
   * Turn the LED off.
   */
  void turn_led_off();

  /**
   * Set the simulated blinker on or off. This is used for testing and debug of
   * synchronization model.
   *
   * @param[in] is_sim Whether or not to set the simulation mode for the Blink
   * class as True or False.
   */
  void set_sim_mode(bool is_sim);

 private:
  /**
   * TUNING PARAMETER: The amount of time the LED will be turned on to signal a
   * flash.
   */
  const long int LED_DURATION_MS = 30;

  /**
   * TUNING PARAMETER: The approximate amount of time it takes between sending
   * the flash request to the hardware and the time it actually turns on. Should
   * be more or less constant but may need to be adjusted especially if there is
   * a "drift" to the synchonized period between the two synchonized fireflies.
   */
  const long int HARDWARE_PROCESSING_MS_ = 35;

  /**
   * Open loop test for the phase_blink function.
   */
  const int LED = 29;

  /**
   * Setting for the simulated Blink mode. Only used for testing and debug of
   * the synchonization model. Does not require a connection to LED hardware.
   */
  bool is_sim_ = false;

  /**
   * Time point at which the LED has been triggered.
   */
  std::chrono::time_point<std::chrono::high_resolution_clock> led_trigger_tm_ =
      std::chrono::high_resolution_clock::now();

  /**
   * Current blink phase.
   */
  long int phase_ = 0;

  /**
   * Current number of flashes.
   */
  long int n_f_ = 5;

  /**
   * TUNING PARAMETER: Set the initial blink period at which the LED will blink.
   * This value should not be more than double of the flashing period of the
   * firefly with which this fireflybot it trying to sync with.
   */
  long int INITIAL_SYNC_PERIOD_MS;

  /**
   * Current blink period.
   */
  long int period_;

  /**
   * Whether or not the LED is currently turned on.
   */
  bool is_led_on_ = false;

  /**
   * Whether in a flash train
   */
  bool flashing_ = false;

  /**
   * Calculate the current phase.
   */
  void calc_phase();
};
}  // namespace fireflybot

#endif /* FIREFLYBOT_BLINK_H */
