#ifndef FIREFLYBOT_BLINK_H  // include guard
#define FIREFLYBOT_BLINK_H

namespace fireflybot {
class Blink {
 public:
  Blink(){};
  ~Blink(){};

  bool initialize();
  void set_phase(double phase);

 private:
  double phase_;

  void led_on();
  void led_off();
};
}  // namespace fireflybot

#endif /* FIREFLYBOT_BLINK_H */