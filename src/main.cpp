#include <iostream>

#include "Sync.h"

using namespace fireflybot;

int main() {
  std::cout << "Starting fireflybot!" << std::endl;

  Sync sync = Sync();
  if (sync.initialize() == true) {
    sync.start();
  }

  std::cout << "Exiting fireflybot!" << std::endl;

  return 0;
}
