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
  double beta = 0.99; // flash immediately after seeing
  double denom = 200; // 7s

  if (is_detected) {
    num_flashes_++;
    Voltage_ = std::min(1.0, Voltage_ + beta);
  } else {
    double dV = I / denom;
    Voltage_ = std::min(1.0, Voltage_ + dV);
  }
  if (_write_data){
    std::vector<long int> data;
    data.emplace_back(num_flashes_);
    data.emplace_back(blink_.get_period());
    data.emplace_back(blink_.get_nf());
    data.emplace_back(blink_.get_phase());
    data.emplace_back(Voltage_);
    record_data(data);
  }
}

void Sync::adjust_period_kuramoto() {
  std::cout << "Adjusting inter-flash phase" << std::endl;
  // https://github.com/owingit/fireflysync/blob/master/kuramato_better_sensor/kuramato_better_sensor.ino

  num_flashes_++;

  auto detected_tm = std::chrono::high_resolution_clock::now();
  std::string tm_str = serialize_time_point(detected_tm, time_format_);
  detected_tm -= std::chrono::milliseconds(detect_tm_ms_);
  std::cout << "Timestamp: " << tm_str << std::endl;
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

  if (adjust_ms > period) {
    adjust_ms = adjust_ms % period;
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
  std::cout << "new period: " << period << std::endl;
  blink_.set_period(period);

  // if (_write_data){
  //   std::vector<long int> data;
  //   data.emplace_back(num_flashes_);
  //   data.emplace_back(period);
  //   data.emplace_back(period_adjust_ms);
  //   data.emplace_back(blink_.get_phase());
  //   data.emplace_back(phase_adjust_ms);
  //   record_data(data);
  // }

  return;
}

void Sync::start() {
  std::cout << "Starting Sync" << std::endl;
  if (_write_data){ init_record_header(); }

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
        bool is_blink_on = blink_.phase_blink();
        if (_write_data) {
         if (is_blink_on) {
           record_data(blink_.get_led_trigger_tm());
         }
       }

      } break;
      case Model::INTEGRATE_AND_FIRE: {
        if (blink_.get_state()) {
          if (is_detected) { adjust_period_kuramoto();}
          bool is_blink_on = blink_.phase_blink();
          if (is_blink_on) { Voltage_ -= (1.0 / blink_.get_nf());}
          if (Voltage_ <= 0.0) { 
            Voltage_ = 0.0;
            blink_.set_state(false);
            blink_.turn_led_off();
            blink_.set_nf(rand()%(10-4 + 1) + 4);
          }
        //   if (_write_data) {
        //     if (is_blink_on) {
        //       record_data(blink_.get_led_trigger_tm());
        //     }
        //  }
        }
        else {
          integrate_fire(is_detected);
          if (Voltage_ >= 0.99) {
            blink_.set_state(true);
          }
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
  auto tm = std::chrono::high_resolution_clock::now();
  std::string tm_str = serialize_time_point(tm, time_format_);
  sync_data_filename_ = saved_data_folder_ + tm_str + sync_data_filename_;
  std::cout << "Saving sync data in: " << sync_data_filename_ << std::endl;

  std::ofstream file(sync_data_filename_, std::fstream::trunc);

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

  blink_data_filename_ = saved_data_folder_ + tm_str + blink_data_filename_;
  std::cout << "Saving blink data in: " << sync_data_filename_ << std::endl;

  std::ofstream blink_file(blink_data_filename_, std::fstream::trunc);
  blink_file << "time"
             << "\n";
}

void Sync::record_data() {
  std::ofstream file(sync_data_filename_, std::fstream::app);

  auto tm = std::chrono::high_resolution_clock::now();
  std::string tm_str = serialize_time_point(tm, time_format_);
  file << tm_str << ",";
  file << num_flashes_ << ",";
  file << blink_.get_phase() << ",";
  file << Voltage_ << "\n";

  file.close();
}

void Sync::record_data(const std::vector<long int>& data) {
  std::ofstream file(sync_data_filename_, std::fstream::app);

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

void Sync::record_data(
    std::chrono::time_point<std::chrono::high_resolution_clock> tm) {
  std::ofstream file(blink_data_filename_, std::fstream::app);

  std::string tm_str = serialize_time_point(tm, time_format_);
  file << tm_str << "\n";

  file.close();
}

void Sync::set_sim_mode(bool is_sim) {
  blink_.set_sim_mode(true);
  camera_.set_sim_mode(true);
}

void Sync::set_initial_period(long int period) {
  blink_.set_init_sync_period(period);
  blink_.set_period(period);
}

void Sync::set_write_data() { _write_data = true; }
} //namespace fireflybot

