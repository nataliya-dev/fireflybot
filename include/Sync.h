#ifndef FIREFLYBOT_SYNC_H
#define FIREFLYBOT_SYNC_H

#include "Blink.h"
#include "Camera.h"

using time_point = std::chrono::system_clock::time_point;
namespace fireflybot {

enum class Status { ON = 0, OFF };
enum class Model { KURAMOTO = 0, INTEGRATE_AND_FIRE };

class Sync {
 public:
  Sync();
  ~Sync(){};

  bool initialize();
  void start();

  static Status STATUS;
  static Model MODEL;

  void set_sim_mode(bool is_sim);

 private:
  // parameters for Kuramotor model
  const int PHASE_SHIFT_FACTOR = 3;
  const int PERIOD_CHANGE_FACTOR = 5;

  // parameters for Integrate and Fire model
  double Voltage_ = 0.5;

  long int num_flashes_ = 0;
  long int detect_tm_ms_ = 0;

  Camera camera_;
  Blink blink_;

  const std::string saved_data_filename_ = "../scripts/sync_data.csv";
  const std::string time_format_ = "%Y-%m-%d %H:%M:%S";

  long int clip(long int n, long int lower, long int upper);
  void adjust_period_kuramoto();
  void adjust_period_integrate_fire();

  void init_record_header();
  void record_data();
  void record_data(const std::vector<long int>& data);
  std::string serialize_time_point(const time_point& time,
                                   const std::string& format);

  void integrate_fire(bool is_detected);
};
}  // namespace fireflybot

#endif /* FIREFLYBOT_SYNC_H */
