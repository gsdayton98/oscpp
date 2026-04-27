// -*- mode: c++ -*-
// @copyright 2021 Glen S. Dayton. All rights reserved,

//// StopWatch class
//
//  Simple utility for timing sections of code.

#ifndef STOPWATCH_HPP_INCL
#define STOPWATCH_HPP_INCL
#include <chrono>

namespace oscpp {
    class __attribute__((visibility("default"))) StopWatch {
        std::chrono::steady_clock::time_point start;

    public:
        /**
         * @brief Construct a new StopWatch object and start it.
         */
        StopWatch() : start{std::chrono::steady_clock::now()}
        {}

        /**
         * @brief Reset the stopwatch to the current time.
         */
        auto reset() -> void
        { start = std::chrono::steady_clock::now(); }

        /**
         * @brief Read the elapsed time since the stopwatch was started.
         * @return Elapsed time in seconds.
         */
        [[nodiscard]] auto read() const -> double;
    };
}
#endif // STOPWATCH_HPP_INCL
