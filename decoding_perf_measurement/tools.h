#pragma once

#include <chrono>
#include <string>

namespace example {

   struct timer {
      std::chrono::time_point<std::chrono::high_resolution_clock> m_t0;
      std::string m_msg;
      double* m_result_target;
      timer(const std::string& msg, double* result_ptr = nullptr);
      timer(double* result_ptr);
      ~timer();
   };

   struct time_result {
      double avg;
      double std;
   };

   auto get_time_result(const std::vector<double>& times) -> time_result;

}
