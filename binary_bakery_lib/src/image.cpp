#include <binary_bakery_lib/image.h>

#include <binary_bakery_lib/file_tools.h>

#include <stb_image.h>
#include <fmt/format.h>

namespace
{

   auto setup_stb_load_flipping(
      const bb::image_vertical_direction direction
   ) -> void
   {
      // For STB, flip=true means bottom to top
      const bool flip = direction == bb::image_vertical_direction::bottom_to_top;
      stbi_set_flip_vertically_on_load(flip);
   }

} // namespace {}


template<int bpp>
bb::image<bpp>::image(
   const abs_file_path& file,
   const image_dimensions& image_dim,
   const image_vertical_direction direction
)
   : m_width(image_dim.width)
   , m_height(image_dim.height)
   , m_pixels(this->get_byte_count(), color<bpp>{ no_init{} })
{
   setup_stb_load_flipping(direction);

   image_dimensions redundand_dim;
   unsigned char* data = stbi_load(
      file.get_path().string().c_str(),
      &redundand_dim.width,
      &redundand_dim.height,
      &redundand_dim.bpp,
      0
   );
   if (data == nullptr)
   {
      const std::string msg = fmt::format("stb_image couldn't load file {}", file.get_path().string());
      throw std::runtime_error(msg);
   }
   if (redundand_dim != image_dim)
   {
      const std::string msg = fmt::format("Second image read returned different dimensions. File: {}", file.get_path().string());
      throw std::runtime_error(msg);
   }
   if (image_dim.bpp != bpp)
   {
      const std::string msg = fmt::format("Explicit bpp parameter is different from file bpp. File: {}.", file.get_path().string());
      throw std::runtime_error(msg);
   }

   std::memcpy(m_pixels.data(), data, get_byte_count());
   stbi_image_free(data);
}
template bb::image<1>::image(const abs_file_path&, const image_dimensions&, const image_vertical_direction);
template bb::image<2>::image(const abs_file_path&, const image_dimensions&, const image_vertical_direction);
template bb::image<3>::image(const abs_file_path&, const image_dimensions&, const image_vertical_direction);
template bb::image<4>::image(const abs_file_path&, const image_dimensions&, const image_vertical_direction);


template<int bpp>
bb::image<bpp>::image(
   const abs_file_path& file,
   const image_vertical_direction direction
)
   : image<bpp>(file, get_image_dimensions(file), direction)
{
   
}
template bb::image<1>::image(const abs_file_path&, const image_vertical_direction);
template bb::image<2>::image(const abs_file_path&, const image_vertical_direction);
template bb::image<3>::image(const abs_file_path&, const image_vertical_direction);
template bb::image<4>::image(const abs_file_path&, const image_vertical_direction);


auto bb::get_image_dimensions(
   const abs_file_path& file
) -> image_dimensions
{
   image_dimensions dimensions;
   const int return_value = stbi_info(
      file.get_path().string().c_str(),
      &dimensions.width,
      &dimensions.height,
      &dimensions.bpp
   );
   if (return_value == 0)
   {
      const std::string msg = fmt::format("stb_info couldn't load file {}", file.get_path().string());
      throw std::runtime_error(msg);
   }
   return dimensions;
}
