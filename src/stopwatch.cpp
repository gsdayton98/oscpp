// -*- mode: c++ -*-;
// @copyright 2021 Glen S. Dayton. Rights reserved according to included license.
#include "stopwatch.hpp"
using namespace std::chrono;

 auto oscpp::StopWatch::read() const -> double {
  const steady_clock::time_point stopwatch_stop = steady_clock::now();
  const steady_clock::duration time_span = stopwatch_stop - start;
  return duration_cast<duration<double> >(time_span).count();
}

