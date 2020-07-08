/**
 * Provides some timing functions
 */
#pragma once
#ifndef TIMING_HPP
#define TIMING_HPP
#include <chrono>

class Timing {
public:
  // automatically record start time
  Timing();
  
  // get time in milliseconds
  double getRunTime(bool cont=false);

private:
  // MinGW timer is inaccurate, so I have to use steady_clock instead
#ifdef __MINGW32__
  std::chrono::steady_clock::time_point startTime;
#else
  std::chrono::high_resolution_clock::time_point startTime;
#endif
};

#endif
