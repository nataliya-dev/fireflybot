#include <csignal>
#include <iostream>

#include "Sync.h"

using namespace fireflybot;

Sync sync;

void signalHandler(int signum) {
  std::cout << "Interrupt signal (" << signum << ") received.\n";
  sync.STATUS = Status::OFF;
  exit(signum);
}

int main() {
  std::cout << "Starting fireflybot!" << std::endl;

  signal(SIGINT, signalHandler);

  sync = Sync();
  if (sync.initialize() == true) {
    sync.start();
  }

  std::cout << "Exiting fireflybot!" << std::endl;

  return 0;
}