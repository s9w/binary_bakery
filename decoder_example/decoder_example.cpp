#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <chrono>
#include <iostream>
#include <format>
#include <fstream>


#include "data_store.h"
#include "tools.h"


namespace example {

   auto from_stb_image(
      const char* filename
   ) -> std::vector<user_type>
   {
      int width, height, bpp;
      unsigned char* data = nullptr;
      data = stbi_load(filename, &width, &height, &bpp, 0);
      if (data == nullptr)
         std::terminate();
      if (bpp != sizeof(user_type))
         std::terminate();
      const int pixel_count = width * height;
      const int byte_count = pixel_count * 3;
      std::vector<user_type> result(pixel_count);
      std::memcpy(result.data(), data, byte_count);
      return result;
   }


   template<typename T>
   auto repeat(
      const T& lambda,
      std::vector<user_type>& storage
   ) -> time_result
   {
      constexpr int n = 300;
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


int main()
{
   using namespace example;
   std::ofstream filestream("data_ RENAME .txt");
   for (const int size : {192, 3072, 49152, 240000, 480000, 3145728})
   {
      filestream << size << ", ";
      const std::string filename = std::format("{}.png", size);
      time(filename, filestream);
   }

   return 0;
}
