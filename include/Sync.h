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
  const int PHASE_SHIFT_FACTOR = 20;
  const int PERIOD_CHANGE_FACTOR = 5;

  Camera camera_;
  Blink blink_;

  void adjust_period_kuramoto();
};
}  // namespace fireflybot

#endif /* FIREFLYBOT_SYNC_H */