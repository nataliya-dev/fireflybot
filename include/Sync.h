#ifndef FIREFLYBOT_SYNC_H  // include guard
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
  Camera camera_;
  Blink blink_;

  void adjust_phase_kuramoto();
};
}  // namespace fireflybot

#endif /* FIREFLYBOT_SYNC_H */