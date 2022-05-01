#include <algorithm>
#include <csignal>
#include <iostream>

#include "Blink.h"
#include "Camera.h"
#include "Sync.h"

using namespace fireflybot;

Sync sync;

void signalHandler(int signum) {
  std::cout << "Interrupt signal (" << signum << ") received.\n";
  sync.STATUS = Status::OFF;
  exit(signum);
}

bool cmdOptionExists(char** begin, char** end, const std::string& option) {
  return std::find(begin, end, option) != end;
}

int main(int argc, char* argv[]) {
  std::cout << "Starting fireflybot!" << std::endl;

  signal(SIGINT, signalHandler);

  if (cmdOptionExists(argv, argv + argc, "-if")) {
    std::cout << "Running synchonization Integrate and Fire module"
              << std::endl;
    sync.MODEL = Model::INTEGRATE_AND_FIRE;
  } else {
    std::cout << "Running synchonization Kuramoto module" << std::endl;
    sync.MODEL = Model::KURAMOTO;
  }

  if (cmdOptionExists(argv, argv + argc, "-tb")) {
    std::cout << "Testing blinker module" << std::endl;
    Blink blinker;
    if (blinker.initialize()) {
      blinker.test_phase_blink();
    }
  } else if (cmdOptionExists(argv, argv + argc, "-tc")) {
    std::cout << "Testing camera module" << std::endl;
    Camera camera;
    if (camera.initialize()) {
      camera.test_camera();
    }
  } else if (cmdOptionExists(argv, argv + argc, "-sim")) {
    std::cout << "Fireflybot simulation mode" << std::endl;
    sync.set_sim_mode(true);
    if (sync.initialize() == true) {
      sync.start();
    }
  } else {
    std::cout << "Running synchonization module" << std::endl;
    if (sync.initialize() == true) {
      sync.start();
    }
  }

  std::cout << "Exiting fireflybot!" << std::endl;

  return 0;
}
