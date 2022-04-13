#ifndef FIREFLYBOT_SYNC_H
#define FIREFLYBOT_SYNC_H

#include "Blink.h"
#include "Camera.h"

namespace fireflybot {

enum class Status { ON = 0, OFF };

class Sync {
 public:
  Sync();
  ~Sync(){};

  bool initialize();
  void start();

  static Status STATUS;

 private:
  const int PHASE_SHIFT_FACTOR = 3;
  const int PERIOD_CHANGE_FACTOR = 7;

  Camera camera_;
  Blink blink_;

  const std::string saved_data_filename = "data.csv";
  // https://www.gormanalysis.com/blog/reading-and-writing-csv-files-with-cpp/

  void adjust_period_kuramoto();
  long int clip(long int n, long int lower, long int upper);
};
}  // namespace fireflybot

#endif /* FIREFLYBOT_SYNC_H */
