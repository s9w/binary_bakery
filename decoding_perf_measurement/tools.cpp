#include "tools.h"

#include <iostream>

#include <fmt/format.h>


example::timer::timer(const std::string& msg, double* result_ptr /*= nullptr*/)
   : m_t0(std::chrono::high_resolution_clock::now())
   , m_msg(msg)
   , m_result_target(result_ptr)
{

}


example::timer::~timer()
{
   const auto t1 = std::chrono::high_resolution_clock::now();
   const auto us = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - m_t0).count();
   const double ms = us / 1'000'000.0;
   if (m_msg.empty() == false)
      std::cout << fmt::format("{}: {}ms\n", m_msg, ms);
   if (m_result_target != nullptr)
      *m_result_target = ms;
}


example::timer::timer(double* result_ptr) : m_t0(std::chrono::high_resolution_clock::now())
, m_result_target(result_ptr)
{

}


auto example::get_time_result(const std::vector<double>& times) -> time_result
{
   double mean = 0.0;
   for (const double value : times)
      mean += value;
   mean = mean / times.size();

   double squares = 0.0;
   for (const double value : times) {
      const double term = value - mean;
      squares += term * term;
   }
   const double std = std::sqrt(squares / (times.size() - static_cast<double>(1.0)));
   return { mean, std };
}
