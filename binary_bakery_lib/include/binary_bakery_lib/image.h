#pragma once

#include <compare>
#include <vector>
#include <cstring> // memcpy

#include <binary_bakery_lib/color.h>


namespace bb
{
   struct abs_file_path;

   struct image_dimensions {
      int width = 0;
      int height = 0;
      int bpp = 0;

      auto operator<=>(const image_dimensions&) const = default;
   };

   template<int bpp>
   struct image {
      static constexpr int channels = bpp;
      using color_type = color<bpp>;

      int m_width = 0;
      int m_height = 0;
      std::vector<color_type> m_pixels{};

      explicit image(const abs_file_path& file, const image_dimensions& image_dim, const image_vertical_direction direction);
      explicit image(const abs_file_path& file, const image_vertical_direction direction);
      [[nodiscard]] auto get_byte_count() const -> int;
      [[nodiscard]] auto get_element_count() const -> int;
      [[nodiscard]] auto operator[](const int index) const -> const color_type&;
      [[nodiscard]] auto operator[](const int index) -> color_type&;
      [[nodiscard]] auto begin() const -> auto;
      [[nodiscard]] auto end() const -> auto;
      auto operator<=>(const image<bpp>&) const = default;
   };

   // To instantiate the templated image type, it's necessary to first find out the images bpp
   // before reading.
   [[nodiscard]] auto get_image_dimensions(const abs_file_path& file) -> image_dimensions;

   template<int bpp>
   [[nodiscard]] auto get_image_bytestream(const image<bpp>& image) -> std::vector<uint8_t>;

}


template <int bpp>
auto bb::image<bpp>::get_byte_count() const -> int
{
   return get_element_count() * sizeof(color_type);
}


template <int bpp>
auto bb::image<bpp>::get_element_count() const -> int
{
   return m_width * m_height;
}


template <int bpp>
auto bb::image<bpp>::operator[](const int index) const -> const color_type&
{
   return m_pixels[index];
}


template <int bpp>
auto bb::image<bpp>::operator[](const int index) -> color_type&
{
   return m_pixels[index];
}


template <int bpp>
auto bb::image<bpp>::begin() const -> auto
{
   return std::begin(m_pixels);
}


template <int bpp>
auto bb::image<bpp>::end() const -> auto
{
   return std::end(m_pixels);
}


template<int bpp>
auto bb::get_image_bytestream(
   const image<bpp>& image
) -> std::vector<uint8_t>
{
   std::vector<uint8_t> result(image.get_byte_count());
   std::memcpy(result.data(), image.m_pixels.data(), image.get_byte_count());
   return result;
}
