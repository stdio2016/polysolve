#include <chrono>
#include "Timing.hpp"

Timing::Timing() {
// MinGW timer is inaccurate, so I have to use steady_clock instead
#ifdef __MINGW32__
  startTime = std::chrono::steady_clock::now();
#else
  startTime = std::chrono::high_resolution_clock::now();
#endif
}

double Timing::getRunTime(bool cont) {
// MinGW timer is inaccurate, so I have to use steady_clock instead
#ifdef __MINGW32__
  auto now = std::chrono::steady_clock::now();
#else
  auto now = std::chrono::high_resolution_clock::now();
#endif
  std::chrono::duration<double, std::milli> t = now - startTime;
  if (!cont) {
    startTime = now;
  }
  return t.count();
}
