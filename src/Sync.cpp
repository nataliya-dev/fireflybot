#include "Sync.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
namespace fireflybot {

Status Sync::STATUS = Status::ON;
Model Sync::MODEL = Model::KURAMOTO;

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

long int Sync::clip(long int n, long int lower, long int upper) {
  return std::max(lower, std::min(n, upper));
}

void Sync::integrate_fire(bool is_detected) {
  // https://github.com/owingit/fireflysync/blob/master/integrate_and_fire/integrate_and_fire.ino

  // MODEL params-----
  double I = 1.0;
  double eta = 1.0;
  double beta = 0.65;
  double N = 10.0;
  double N_mult = 500.0;

  if (is_detected) {
    num_flashes_++;
    Voltage_ = std::min(1.0, Voltage_ + (beta / N));
  } else {
    double dV = I - eta * Voltage_;
    // std::cout << "dV: " << dV << std::endl;
    Voltage_ = Voltage_ + (dV / (N_mult * N));
  }
  // std::cout << "Voltage: " << Voltage_ << std::endl;
  // std::cout << "Phase: " << blink_.get_phase() << std::endl;

  record_data();
}

void Sync::adjust_period_kuramoto() {
  std::cout << "Adjusting phase based on kuramoto model" << std::endl;
  // https://github.com/owingit/fireflysync/blob/master/kuramato_better_sensor/kuramato_better_sensor.ino

  num_flashes_++;

  auto detected_tm = std::chrono::high_resolution_clock::now();
  detected_tm -= std::chrono::milliseconds(detect_tm_ms_);
  std::cout << "detect_tm_ms_: " << detect_tm_ms_ << std::endl;
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
  // period = clip(
  //     period, blink_.get_init_sync_period() - blink_.get_init_sync_period() /
  //     2, blink_.get_init_sync_period() + blink_.get_init_sync_period() / 2);

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
  init_record_header();

  while (STATUS == Status::ON) {
    bool is_detected = camera_.is_flash_detected(detect_tm_ms_);

    switch (MODEL) {
      case Model::KURAMOTO: {
        if (is_detected == true) {
          auto start_tm = std::chrono::high_resolution_clock::now();
          adjust_period_kuramoto();
          auto end_tm = std::chrono::high_resolution_clock::now();
          auto adjust_tm_ms =
              std::chrono::duration<double, std::milli>(end_tm - start_tm)
                  .count();
          std::cout << "adjust_tm_ms: " << adjust_tm_ms << std::endl;
        }
        blink_.phase_blink();
      } break;
      case Model::INTEGRATE_AND_FIRE: {
        integrate_fire(is_detected);
        if (Voltage_ >= 0.999) {
          blink_.burst_blink();
          Voltage_ = 0.0;
        }
      } break;
    }
  }

  std::cout << "Exiting Sync" << std::endl;
}

std::string Sync::serialize_time_point(const time_point& time,
                                       const std::string& format) {
  std::time_t tt = std::chrono::system_clock::to_time_t(time);
  // std::tm tm = *std::gmtime(&tt);  // GMT (UTC)
  std::tm tm = *std::localtime(&tt);
  auto ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(
          time.time_since_epoch()) -
      std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch());
  std::stringstream ss;
  ss << std::put_time(&tm, format.c_str());
  ss << "." << ms.count();
  return ss.str();
}

void Sync::init_record_header() {
  std::ofstream file(saved_data_filename_, std::fstream::trunc);

  switch (MODEL) {
    case Model::KURAMOTO: {
      if (!file.is_open())
        throw std::runtime_error("Could not open record file");
      file << "time"
           << ","
           << "num_flashes"
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
    case Model::INTEGRATE_AND_FIRE: {
      file << "time"
           << ","
           << "num_flashes"
           << ","
           << "phase"
           << ","
           << "V\n";
      file.close();
    }
  }
}

void Sync::record_data() {
  std::ofstream file(saved_data_filename_, std::fstream::app);

  auto tm = std::chrono::high_resolution_clock::now();
  std::string tm_str = serialize_time_point(tm, time_format_);
  file << tm_str << ",";
  file << num_flashes_ << ",";
  file << blink_.get_phase() << ",";
  file << Voltage_ << "\n";

  file.close();
}

void Sync::record_data(const std::vector<long int>& data) {
  std::ofstream file(saved_data_filename_, std::fstream::app);

  std::size_t num_data_pts = data.size();
  auto tm = std::chrono::high_resolution_clock::now();
  std::string tm_str = serialize_time_point(tm, time_format_);
  file << tm_str << ",";
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