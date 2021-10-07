#pragma once

#include <compare>
#include <vector>
#include <string>
#include <format>

#include "tools.h"

#include "stb_image.h"


namespace inliner {

   template<int bpp>
   struct color {
      uint8_t m_components[bpp];

      auto operator<=>(const color<bpp>&) const = default;
      constexpr color(no_init){ }
      constexpr color(uint8_t r) requires(bpp == 1)
         : m_components{ r }
      {}
      constexpr color(uint8_t r, uint8_t a) requires(bpp == 2)
         : m_components{ r, a }
      {}
      constexpr color(uint8_t r, uint8_t g, uint8_t b) requires(bpp==3)
         : m_components{ r, g, b }
      {}
      constexpr color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) requires(bpp == 4)
         : m_components{ r, g, b, a }
      {}
      [[nodiscard]] constexpr auto operator[](const int index) const -> uint8_t {
         return m_components[index];
      }
   };

   // Deduction guides
   color(uint8_t) -> color<1>;
   color(uint8_t, uint8_t) -> color<2>;
   color(uint8_t, uint8_t, uint8_t) -> color<3>;
   color(uint8_t, uint8_t, uint8_t, uint8_t) -> color<4>;

   template<int bpp>
   struct image {
      static constexpr int channels = bpp;
      using color_type = color<bpp>;

      int m_width = 0;
      int m_height = 0;
      std::vector<color_type> m_pixels;

      explicit image(const int w, const int h, const unsigned char* data);
      [[nodiscard]] auto to_uint64() const -> std::vector<uint64_t>;
      [[nodiscard]] auto get_byte_count() const -> int;
      [[nodiscard]] auto get_pixel_count() const -> int;
      [[nodiscard]] auto operator[](const int index) const -> const color_type&;
      [[nodiscard]] auto operator[](const int index) -> color_type&;
      [[nodiscard]] auto begin() const -> auto;
      [[nodiscard]] auto end() const -> auto;
   };

   template<int bpp>
   [[nodiscard]] auto get_image(const std::string& filename) -> image<bpp>;

}


template<int bpp>
auto inliner::get_image(const std::string& filename) -> image<bpp>
{
   stbi_set_flip_vertically_on_load(true);
   int width, height, components;
   unsigned char* data = stbi_load(filename.c_str(), &width, &height, &components, 0);
   if (data == NULL) {
      const std::string msg = std::format("Couldn't open file {}", filename);
      throw std::runtime_error(msg);
   }
   if (components != bpp) {
      const std::string msg = std::format("wrong bpp count. Expected {}, got {}", bpp, components);
      throw std::runtime_error(msg);
   }

   const image<bpp> result{ width, height, data };
   stbi_image_free(data);
   return result;
}


template <int bpp>
inliner::image<bpp>::image(const int w, const int h, const unsigned char* data): m_width(w)
   , m_height(h)
   , m_pixels(m_width* m_height, no_init{})
{
   std::memcpy(m_pixels.data(), data, get_byte_count());
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
   return m_width * m_height;
}


template <int bpp>
auto inliner::image<bpp>::operator[](const int index) const -> const color_type&
{
   return m_pixels[index];
}


template <int bpp>
auto inliner::image<bpp>::operator[](const int index) -> color_type&
{
   return m_pixels[index];
}


template <int bpp>
auto inliner::image<bpp>::begin() const -> auto
{
   return std::begin(m_pixels);
}


template <int bpp>
auto inliner::image<bpp>::end() const -> auto
{
   return std::end(m_pixels);
}
