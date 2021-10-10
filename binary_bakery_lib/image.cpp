#include "image.h"

#include <stb/stb_image.h>


template<int bpp>
auto bb::get_image(const fs::path& path) -> image<bpp>
{
   stbi_set_flip_vertically_on_load(true);
   int width, height, components;
   unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &components, 0);
   if (data == nullptr) {
      const std::string msg = std::format("Couldn't open file {}", path.string());
      throw std::runtime_error(msg);
   }
   if (components != bpp) {
      const std::string msg = std::format("wrong bpp count. Expected {}, got {}", bpp, components);
      throw std::runtime_error(msg);
   }

   image<bpp> result{ width, height, data };
   stbi_image_free(data);
   return result;
}

template auto bb::get_image<1>(const fs::path& path) -> bb::image<1>;
template auto bb::get_image<2>(const fs::path& path) -> bb::image<2>;
template auto bb::get_image<3>(const fs::path& path) -> bb::image<3>;
template auto bb::get_image<4>(const fs::path& path) -> bb::image<4>;
