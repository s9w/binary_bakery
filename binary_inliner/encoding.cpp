#include "encoding.h"

#include <format>
#include <fstream>
#include <optional>
#include <iostream>

#include "tools.h"
#include "color.h"
#include "binary_image_tools.h"

#include "stb_image.h"


namespace {

   using namespace inliner;

   [[nodiscard]] constexpr auto is_image(const std::string& filename) -> bool
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
      return { buffer, static_cast<int>(byte_count), binary_type{} };
   }


   template<int bpp>
   [[nodiscard]] auto get_image_meta(
      const image_view<bpp>& image
   ) -> content_meta
   {
      const std::optional<color_pair<bpp>> color_pair = get_color_pair<bpp>(image);
      if (color_pair.has_value())
      {
         return dual_image_type<bpp>{
            image.width,
            image.height,
            color_pair.value().color0,
            color_pair.value().color1
         };
      }
      else
      {
         return naive_image_type{ image.width, image.height, bpp};
      }
   }


   [[nodiscard]] auto get_image_payload(const std::string& filename) -> payload
   {
      int width, height, components;
      unsigned char* data = stbi_load(filename.c_str(), &width, &height, &components, 0);
      if (data == NULL) {
         const std::string msg = std::format("Couldn't open file {}", filename);
         throw std::runtime_error(msg);
      }

      const int byte_count = width * height * components;
      

      

      const content_meta meta = [&]() {
         switch (components) {
         case 1:
            return get_image_meta(image_view<1>{ width, height, data });
         case 2:
            return get_image_meta(image_view<2>{ width, height, data });
         case 3:
            return get_image_meta(image_view<3>{ width, height, data });
         case 4:
            return get_image_meta(image_view<4>{ width, height, data });
         default:
            std::cout << "unexpected\n";
            return content_meta{ naive_image_type{} };
         }
            
      }();

      stbi_image_free(data);

      std::vector<uint64_t> image_binary(inliner::get_symbol_count<uint64_t>(byte_count));
      std::memcpy(image_binary.data(), data, byte_count);
      return { image_binary, byte_count, meta };
   }

}


auto inliner::encode(const std::string& filename) -> payload
{
   if (is_image(filename))
      return get_image_payload(filename);
   else
      return get_binary_file_payload(filename.c_str());
}
