#pragma once

#include <compare>
#include <vector>
#include <string>
#include <format>
#include <optional>
#include <filesystem>

namespace fs = std::filesystem;

#include "color.h"
#include "tools.h"
#include "content_meta.h"

namespace bb
{

   template<int bpp>
   struct image {
      static constexpr int channels = bpp;
      using color_type = color<bpp>;

      int m_width = 0;
      int m_height = 0;
      std::vector<color_type> m_pixels;

      explicit image(const int w, const int h, const unsigned char* data);
      [[nodiscard]] auto get_byte_count() const -> int;
      [[nodiscard]] auto get_pixel_count() const -> int;
      [[nodiscard]] auto operator[](const int index) const -> const color_type&;
      [[nodiscard]] auto operator[](const int index) -> color_type&;
      [[nodiscard]] auto begin() const -> auto;
      [[nodiscard]] auto end() const -> auto;
      auto operator<=>(const image<bpp>&) const = default;
   };

   template<int bpp>
   [[nodiscard]] auto get_image(const fs::path& path) -> image<bpp>;

   template<int bpp>
   [[nodiscard]] auto get_image_bytestream(const image<bpp>& image) -> std::vector<uint8_t>;

}



template <int bpp>
bb::image<bpp>::image(const int w, const int h, const unsigned char* data): m_width(w)
   , m_height(h)
   , m_pixels(m_width* m_height, no_init{})
{
   std::memcpy(m_pixels.data(), data, get_byte_count());
}


template <int bpp>
auto bb::image<bpp>::get_byte_count() const -> int
{
   return get_pixel_count() * sizeof(color_type);
}


template <int bpp>
auto bb::image<bpp>::get_pixel_count() const -> int
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
