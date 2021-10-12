#include <bit>     // For std::bit_cast
#include <cstdint> // For sized types
#include <string>  // For std::memcpy
#include <vector>
#include <array>


#define STB_IMAGE_IMPLEMENTATION
#include "../libs/stb/stb_image.h"

#include <chrono>
#include <iostream>
struct timer {
   std::chrono::time_point<std::chrono::high_resolution_clock> m_t0;
   std::string m_msg;
   double* m_result_target;
   timer(const std::string& msg, double* result_ptr = nullptr)
      : m_t0(std::chrono::high_resolution_clock::now())
      , m_msg(msg)
      , m_result_target(result_ptr)
   {}
   timer(double* result_ptr)
      : m_t0(std::chrono::high_resolution_clock::now())
      , m_result_target(result_ptr)
   {}
   ~timer()
   {
      const auto t1 = std::chrono::high_resolution_clock::now();
      const auto us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - m_t0).count();
      const double ms = us / 1000.0;
      if(m_msg.empty() == false)
         std::cout << std::format("{}: {}ms\n", m_msg, ms);
      if (m_result_target != nullptr)
         *m_result_target = ms;
   }
};

struct user_type {
   uint8_t r{}, g{}, b{};
   auto operator<=>(const user_type&) const = default;
};


auto from_stb_image(const char* filename) -> std::vector<user_type>
{
   int width, height, bpp;
   unsigned char* data = nullptr;
   {
      //timer t("stbi_load");
      data = stbi_load(filename, &width, &height, &bpp, 0);
   }
   if (data == nullptr)
      std::terminate();
   if (bpp != sizeof(user_type))
      std::terminate();
   const int pixel_count = width * height;
   const int byte_count = pixel_count * 3;
   std::vector<user_type> result;
   {
      //timer t("vector resize");
      result.resize(pixel_count);
   }
   {
      //timer t("just memcpy");
      std::memcpy(result.data(), data, byte_count);
   }
   return result;
}

#define BAKERY_PROVIDE_VECTOR
#ifndef ZERO
#include "../binary_bakery_decoder.h"
#include "temp_payload.h"
#endif


int main()
{
   std::vector<user_type> vec;

   double binary_time{}, file_time{};
   {
      timer t("Time to decode from binary", &binary_time);
      #ifndef ZERO
      vec = bb::decode_to_vector<user_type>("123");
      #endif
   }
   {
      // 95% is from stbi_load
      timer t("Time to load from file", &file_time);
      vec = from_stb_image(R"(C:\Dropbox\code\binary_inliner\input_tests\240000.png)");
   }
   std::cout << std::format("Binary takes {:.2f}% of file load.\n", 100.0*binary_time/file_time);

   printf("%llu bytes\n", vec.size());

   return 0;
}
