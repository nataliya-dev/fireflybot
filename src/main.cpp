#include <algorithm>
#include <csignal>
#include <iostream>

#include "Blink.h"
#include "Camera.h"
#include "Sync.h"

using namespace fireflybot;

/**
 *   Default starting period
 **/
const long int DEFAULT_STARTING_PERIOD = 900; //ms
const double DEFAULT_STARTING_BETA = 0.99; //ms

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

  // set starting period from command line
  long int initial_period = DEFAULT_STARTING_PERIOD;
  if (cmdOptionExists(argv, argv + argc, "-isp")) {
    for (int i = 0; i < argc; i++) {
      if (argv[i][0] != '.' || argv[i][0] != '0') {
        if (atol(argv[i]) > 0) {
          initial_period = atol(argv[i]);
          break;
        }
      }
    }
  }
  sync.set_initial_period(initial_period);
  std::cout << "Initial sync period: " << initial_period << "ms"
              << std::endl;

  // set beta from command line
  double beta = DEFAULT_STARTING_BETA;
  if (cmdOptionExists(argv, argv + argc, "-beta")) {
    for (int i = 0; i < argc; i++) {
      if (argv[i][0] == '.' || argv[i][0] == '0') {
        if (atof(argv[i]) > 0) {
          beta = atof(argv[i]);
          break;
        }
      }
    }
  }
  sync.set_beta(beta);
  std::cout << "Beta: " << sync.get_beta() << std::endl;

  // set up writing from command line
  if (cmdOptionExists(argv, argv + argc, "-write")) {
    sync.set_write_data();
  }

  // choose sync model 
  if (cmdOptionExists(argv, argv + argc, "-inf")) {
    std::cout << "Running synchonization Integrate and Fire module"
              << std::endl;
    sync.MODEL = Model::INTEGRATE_AND_FIRE;
  } else {
    std::cout << "Running synchonization Kuramoto module" << std::endl;
    sync.MODEL = Model::KURAMOTO;
  }

  // establish whether testing or running
  if (cmdOptionExists(argv, argv + argc, "-tb")) {
    std::cout << "Testing blinker module" << std::endl;
    Blink blinker;
    blinker.set_init_sync_period(initial_period);
    blinker.set_period(initial_period);
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
