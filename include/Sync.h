#ifndef FIREFLYBOT_SYNC_H  // include guard
#define FIREFLYBOT_SYNC_H

#include "Blink.h"
#include "Camera.h"

namespace fireflybot {
class Sync {
 public:
  Sync(){};
  ~Sync(){};

  bool initialize();
  void start();

 private:
  Camera camera_;
  Blink blink_;

  void adjust_phase_kuramoto();
};
}  // namespace fireflybot

#endif /* FIREFLYBOT_SYNC_H */