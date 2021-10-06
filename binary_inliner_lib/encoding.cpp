#include "encoding.h"

#include <format>
#include <fstream>
#include <optional>
#include <iostream>

#include "tools.h"
#include "image.h"
#include "binary_image_tools.h"

#include "stb_image.h"


namespace {

   using namespace inliner;

   [[nodiscard]] constexpr auto is_image_path(const std::string& filename) -> bool
   {
      if (filename.ends_with("png"))
      {
         return true;
      }
      return false;
   }


   [[nodiscard]] auto get_binary_file_payload(const char* path) -> payload
   {
      std::ifstream file(path, std::ios::ate | std::ios::binary);

      if (file.is_open() == false) {
         const std::string msg = std::format("Couldn't open file {}", path);
         throw std::runtime_error(msg);
      }

      const size_t byte_count = file.tellg();
      const auto element_count = byte_count / sizeof(uint64_t);
      std::vector<uint64_t> buffer(element_count);

      file.seekg(0);
      file.read(reinterpret_cast<char*>(buffer.data()), byte_count);
      return { buffer, static_cast<int>(byte_count), generic_binary{} };
   }


   [[nodiscard]] auto get_image_payload(const std::string& filename) -> payload
   {
      int width, height, components;
      unsigned char* data = stbi_load(filename.c_str(), &width, &height, &components, 0);
      if (data == NULL) {
         const std::string msg = std::format("Couldn't open file {}", filename);
         throw std::runtime_error(msg);
      }

      const payload result = [&]() {
         switch (components) {
         case 1:
            return detail::get_payload<1>(width, height, data);
         case 2:
            return detail::get_payload<2>(width, height, data);
         case 3:
            return detail::get_payload<3>(width, height, data);
         case 4:
            return detail::get_payload<4>(width, height, data);
         default:
            std::cout << "unexpected\n";
            return payload{};
         };
      }();

      stbi_image_free(data);
      return result;
   }

}


auto inliner::encode(const std::string& filename) -> payload
{
   if (is_image_path(filename))
      return get_image_payload(filename);
   else
      return get_binary_file_payload(filename.c_str());
}
