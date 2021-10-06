#pragma once

#include <compare>

#include "tools.h"


namespace inliner {

   template<int bpp>
   struct color {
      uint8_t components[bpp];
      auto operator<=>(const color<bpp>&) const = default;
   };

   template<int bpp>
   struct image {
      static constexpr int channels = bpp;
      using color_type = color<bpp>;

      int m_width = 0;
      int height = 0;
      std::vector<color_type> m_pixels;

      explicit image(const int w, const int h, const unsigned char* data)
         : m_width(w)
         , height(h)
         , m_pixels(m_width* height)
      {
         std::memcpy(m_pixels.data(), data, get_byte_count());
      }

      [[nodiscard]] auto to_uint64() const -> std::vector<uint64_t>;
      [[nodiscard]] auto get_byte_count() const -> int;
      [[nodiscard]] auto get_pixel_count() const -> int;

      [[nodiscard]] auto operator[](const int index) const -> const color_type&
      {
         return m_pixels[index];
      }

      auto begin() const -> auto
      {
         return std::begin(m_pixels);
      }
      auto end() const -> auto
      {
         return std::end(m_pixels);
      }
   };

}


template<int bpp>
auto inliner::image<bpp>::to_uint64() const -> std::vector<uint64_t>
{
   const int ui64_count = get_symbol_count<uint64_t>(get_byte_count());
   std::vector<uint64_t> result(ui64_count);
   std::memcpy(result.data(), m_pixels.data(), get_byte_count());
   return result;
}


template <int bpp>
auto inliner::image<bpp>::get_byte_count() const -> int
{
   return get_pixel_count() * sizeof(color_type);
}


template <int bpp>
auto inliner::image<bpp>::get_pixel_count() const -> int
{
   return m_width * height;
}
