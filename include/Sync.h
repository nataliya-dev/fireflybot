#ifndef FIREFLYBOT_SYNC_H
#define FIREFLYBOT_SYNC_H

#include "Blink.h"
#include "Camera.h"

using time_point = std::chrono::system_clock::time_point;
namespace fireflybot {

/**
 * The status of the sync module.
 */
enum class Status {
  ON = 0,  // normal functioning
  OFF      // stop the processing
};

/**
 * The model that will be used for synchronization.
 */
enum class Model { KURAMOTO = 0, INTEGRATE_AND_FIRE };

class Sync {
 public:
  Sync();
  ~Sync(){};

  /**
   * Initialize the operational parameters of the camera, blink and other
   * classes needed to start the synchronization.
   *
   * @return bool value is True if initialization is a success, False on
   * initialization failure.
   */
  bool initialize();

  /**
   * Start the synchonization loop. Processes frames and tries to understand how
   * to synchonize with a light in its environment.
   */
  void start();

  /**
   * The status of the sync module.
   */
  static Status STATUS;

  /**
   * The model that will be used for synchronization.
   */
  static Model MODEL;

  /**
   * Simulation process of the synchronization. Sets the camera and blinker to
   * also be in the simulation model. Does not require connection to hardware.
   * Only used for debug and testing.
   *
   * @param[in] is_sim True to turn on the simulation mode. False to to turn off
   * the simulation mode.
   */
  void set_sim_mode(bool is_sim);

 private:
  /**
   * TUNING PARAMETER: How much to adjust the blink phase after a flash has been
   * detected.
   */
  const int PHASE_SHIFT_FACTOR = 3;

  /**
   * TUNING PARAMETER: How much to adjust the blink period after a flash has
   * been detected.
   */
  const int PERIOD_CHANGE_FACTOR = 5;

  /**
   * Keep track of the voltage for the integrate and fire model.
   */
  double Voltage_ = 0.5;

  /**
   * The number of flashes that have been detected.
   */
  long int num_flashes_ = 0;

  /**
   * The amount of time it took the camera to detect a flash.
   */
  long int detect_tm_ms_ = 0;

  /**
   * The interface to the Camera hardware and software.
   */
  Camera camera_;

  /**
   * The interface to the hardware and software that flashes a light.
   */
  Blink blink_;

  /**
   * The file name in which syn data will be saved as the process is running.
   */
  const std::string saved_data_filename_ = "../scripts/sync_data.csv";

  /**
   * The time format which is used to timestamp all the data being saved.
   */
  const std::string time_format_ = "%Y-%m-%d %H:%M:%S";

  /**
   * The time format which is used to timestamp all the data being saved.
   */
  long int clip(long int n, long int lower, long int upper);

  /**
   * When a flash is detected, adjust the blinking phase and period based on the
   * kuramoto model.
   */
  void adjust_period_kuramoto();

  /**
   * Use the integrate and fire model to determine whether or not to blink based
   * on the integrate and fire model.
   */
  void adjust_period_integrate_fire();

  /**
   * Initialize the header for the data being recorded in the csv file.
   */
  void init_record_header();

  /**
   * Record data based on the sync model.
   */
  void record_data();

  /**
   * Record data based on the sync model.
   *
   * @param[in] data The vector of data that needs to be recorded. Must
   * correspond to the header.
   */
  void record_data(const std::vector<long int>& data);

  /**
   * Take a time point and convert it into a string. The string is then saved in
   * the csv file,
   *
   * @param[in] time The time point to serialize.
   * @param[in] format The format in which to save the time point.
   *
   * @return The serialized string.
   */
  std::string serialize_time_point(const time_point& time,
                                   const std::string& format);

  /**
   * Process the integrate and fire model.
   *
   * @param[in] is_detected Whether or not a light has been detected by the
   * camera.
   */
  void integrate_fire(bool is_detected);
};
}  // namespace fireflybot

#endif /* FIREFLYBOT_SYNC_H */
