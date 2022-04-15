#include "Sync.h"

#include <chrono>
#include <fstream>
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

  init_record_header();

  return true;
}

long int Sync::clip(long int n, long int lower, long int upper) {
  return std::max(lower, std::min(n, upper));
}

void Sync::adjust_period_integrate_fire() {}

void Sync::adjust_period_kuramoto() {
  std::cout << "Adjusting phase based on kuramoto model" << std::endl;
  // https://github.com/owingit/fireflysync/blob/master/kuramato_better_sensor/kuramato_better_sensor.ino

  num_flashes_++;

  auto detected_tm = std::chrono::high_resolution_clock::now();
  auto led_trigger_tm = blink_.get_led_trigger_tm();
  long int elapsed_time_ms =
      std::chrono::duration<double, std::milli>(detected_tm - led_trigger_tm)
          .count();
  std::cout << "elapsed_time_ms: " << elapsed_time_ms << std::endl;

  long int adjust_ms = 0;
  long int period = blink_.get_period();
  std::cout << "period: " << period << std::endl;

  if (elapsed_time_ms < period / 2) {
    adjust_ms = elapsed_time_ms;
  } else {
    adjust_ms = elapsed_time_ms - period;
  }
  std::cout << "adjust_ms: " << adjust_ms << std::endl;

  long int phase_adjust_ms = adjust_ms / PHASE_SHIFT_FACTOR;
  std::cout << "phase_adjust_ms: " << phase_adjust_ms << std::endl;
  led_trigger_tm += std::chrono::milliseconds(phase_adjust_ms);
  blink_.set_led_trigger_tm(led_trigger_tm);

  elapsed_time_ms =
      std::chrono::duration<double, std::milli>(detected_tm - led_trigger_tm)
          .count();
  std::cout << "new elapsed_time_ms: " << elapsed_time_ms << std::endl;

  long int period_adjust_ms = adjust_ms / PERIOD_CHANGE_FACTOR;
  std::cout << "period_adjust_ms: " << period_adjust_ms << std::endl;
  period += period_adjust_ms;
  period = clip(
      period, blink_.get_init_sync_period() - blink_.get_init_sync_period() / 2,
      blink_.get_init_sync_period() + blink_.get_init_sync_period() / 2);

  std::cout << "new period: " << period << std::endl;
  blink_.set_period(period);

  std::vector<long int> data;
  data.emplace_back(num_flashes_);
  data.emplace_back(period);
  data.emplace_back(period_adjust_ms);
  data.emplace_back(blink_.get_phase());
  data.emplace_back(phase_adjust_ms);
  record_data(data);

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

void Sync::init_record_header() {
  std::ofstream file(saved_data_filename_);
  if (!file.is_open()) throw std::runtime_error("Could not open record file");
  file << "num_flashes"
       << ","
       << "period"
       << ","
       << "period_adjust"
       << ","
       << "phase"
       << ","
       << "phase_adjust\n";
  file.close();
}

void Sync::record_data(const std::vector<long int>& data) {
  std::ofstream file(saved_data_filename_, std::fstream::app);

  std::size_t num_data_pts = data.size();
  for (std::size_t i = 0; i < num_data_pts; i++) {
    if (i == num_data_pts - 1) {
      file << data[i] << "\n";
    } else {
      file << data[i] << ",";
    }
  }
  file.close();
}

void Sync::set_sim_mode(bool is_sim) {
  blink_.set_sim_mode(true);
  camera_.set_sim_mode(true);
}

}  // namespace fireflybot