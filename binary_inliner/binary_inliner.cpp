#include <format>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstring> // memcpy
#include <variant>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "tools.h"
#include "payload.h"
#include "encoding.h"
#include "decoding.h"


static_assert(inliner::get_symbol_count<uint64_t>(8) == 1);
static_assert(inliner::get_symbol_count<uint64_t>(9) == 2);

namespace inliner {



   //auto get_image_as_binary(const char* filename) -> payload
   //{
   //   int width, height, components;
   //   unsigned char* data = stbi_load(filename, &width, &height, &components, 0);
   //   if (data == NULL) {
   //      const std::string msg = std::format("Couldn't open file {}", filename);
   //      throw std::runtime_error(msg);
   //   }

   //   const int byte_count = width * height * components;
   //   std::vector<uint64_t> result(get_symbol_count<uint64_t>(byte_count));
   //   std::memcpy(result.data(), data, byte_count);

   //   stbi_image_free(data);

   //   return { result, byte_count };
   //}

   struct dummy_color {
      uint8_t r, g, b;
   };

   template<typename color_type>
   struct image {
      std::vector<color_type> pixels;
      int width = 0;
      int height = 0;
   };

   template<typename color_type>
   [[nodiscard]] auto decode_image(const payload& input) -> std::vector<color_type>
   {
      const int element_count = get_symbol_count<color_type>(input.byte_count);
      if (input.byte_count % sizeof(color_type) > 0)
      {
         const std::string msg = std::format("Can't decode into that type");
         throw std::runtime_error(msg);
      }
      std::vector<dummy_color> result(element_count);
      std::memcpy(result.data(), input.data.data(), input.byte_count);
      return result;
   }


   //template<typename underlying_type>
   //[[nodiscard]] auto get_binary_file_contents(const char* path) -> std::vector<underlying_type>
   //{
   //   std::ifstream file(path, std::ios::ate | std::ios::binary);

   //   if (file.is_open() == false) {
   //      const std::string msg = std::format("Couldn't open file {}", path);
   //      throw std::runtime_error(msg);
   //   }

   //   const size_t byte_count = file.tellg();
   //   const auto element_count = byte_count / sizeof(underlying_type);
   //   std::vector<underlying_type> buffer(element_count);

   //   file.seekg(0);
   //   file.read(reinterpret_cast<char*>(buffer.data()), byte_count);
   //   return buffer;
   //}



}


int main(int argc, char* argv[]) {
   for (int i = 0; i < argc; ++i)
      std::cout << std::format("{}: {}\n", i, argv[i]);

   if (argc != 2) {
      std::cout << "argument count != 2\n" << std::endl;
      return 0;
   }

   stbi_set_flip_vertically_on_load(true);

   //const inliner::payload image_vector = inliner::get_image_as_binary("C:\\Dropbox\\code\\binary_inliner\\x64\\Debug\\4x4.png");
   //const std::vector<inliner::dummy_color> decoded = inliner::decode_image<inliner::dummy_color>(image_vector);
   return 0;
}
