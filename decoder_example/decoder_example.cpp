#include <bit>     // For std::bit_cast
#include <cstdint> // For sized types
#include <string>  // For std::memcpy
#include <vector>
#include <array>


#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <chrono>
#include <iostream>
#include <format>
#include <fstream>


#include "data_store.h"
#include "tools.h"


namespace example {
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



   template<typename T>
   auto repeat(
      const T& lambda,
      std::vector<user_type>& storage
   ) -> time_result
   {
      constexpr int n = 10;
      std::vector<double> times;
      times.reserve(n);
      
      for (int i = 0; i < n; ++i)
      {
         double time = 0.0;
         {
            timer t("", &time);
            storage = lambda();
         }
         times.emplace_back(time);
      }
      return get_time_result(times);
   }


   auto time(
      const std::string& filename,
      std::ofstream& filestream
   ) -> void
   {
      std::vector<user_type> vec;

      std::string path = R"(C:\Dropbox\code\binary_inliner\input_tests\)" + filename;
      const time_result binary_times = repeat([&]() {return example::decode_to_vector(filename.c_str()); }, vec);
      const time_result stb_times = repeat([&]() {return from_stb_image(path.c_str()); }, vec);
      filestream << binary_times.avg << ", " << binary_times.std << ", " << stb_times.avg << ", " << stb_times.std << "\n";
   }
}

//#define BAKERY_PROVIDE_VECTOR
//#define BAKERY_PROVIDE_STD_ARRAY
//#include "../decoder.h"




int main()
{
   using namespace example;
   std::ofstream filestream("decode_speed_data.txt");
   for (const int size : {192, 3072, 49152, 240000, 480000, 3145728})
   {
      filestream << size << ", ";
      const std::string filename = std::format("{}.png", size);
      time(filename, filestream);
   }
   
   //constexpr bb::header decoded_header = bb::get_header("192.png");

   //std::vector<user_type> vec;

   //double binary_time{}, file_time{};
   //{
   //   timer t("Time to decode from binary", &binary_time);
   //   vec = example::decode_to_vector("192.png");
   //}
   //{
   //   // 95% is from stbi_load
   //   timer t("Time to load from file", &file_time);
   //   vec = from_stb_image(R"(C:\Dropbox\code\binary_inliner\input_tests\240000.png)");
   //}
   //std::cout << std::format("Binary takes {:.2f}% of file load.\n", 100.0*binary_time/file_time);

   //printf("%llu bytes\n", vec.size());

   return 0;
}
