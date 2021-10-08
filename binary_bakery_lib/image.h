#pragma once

#include <compare>
#include <vector>
#include <string>
#include <format>
#include <optional>

#include "color.h"
#include "tools.h"
#include "content_meta.h"

#include "stb_image.h"

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
   };

   template<int bpp>
   [[nodiscard]] auto get_image(const std::string& filename) -> image<bpp>;

   template<int bpp>
   [[nodiscard]] auto get_color_pair(const image<bpp>& image) -> std::optional<color_pair<bpp>>;

   enum class dual_color_name { first, second };

   template<int bpp>
   [[nodiscard]] auto get_indexed_dual_image(
      const image<bpp>& im,
      const color_pair<bpp>& pair
   ) -> std::vector<dual_color_name>;

   template<int bpp>
   [[nodiscard]] auto get_image_bytestream_naive(const image<bpp>& image) -> std::vector<uint8_t>;

   template<int bpp>
   [[nodiscard]] auto get_image_bytestream_dual(const image<bpp>& image, const color_pair<bpp>& pair) -> std::vector<uint8_t>;

   template<int bpp>
   [[nodiscard]] auto get_image_bytestream(const image<bpp>& image, const content_meta& meta) -> std::vector<uint8_t>;

}

namespace bb::detail
{
   template<int bpp>
   [[nodiscard]] auto get_actual_color_pair(
      const image<bpp>& im
   ) -> std::optional<color_pair<bpp>>
   {
      using color_type = image<bpp>::color_type;

      color_type color_A = im[0];
      std::optional<color_type> color_B;
      for (const color_type& color : im)
      {
         if (color == color_A)
         {
            continue;
         }
         if (color_B.has_value() && color == color_B.value())
         {
            continue;
         }
         if (color_B.has_value() == false)
         {
            color_B.emplace(color);
            continue;
         }
         return std::nullopt;
      }

      // Only contains one color
      if (color_B.has_value() == false)
      {
         color_B.emplace(color_A);
      }
      return color_pair<bpp>{color_A, color_B.value()};
   }


   template<int bpp>
   [[nodiscard]] constexpr auto get_indexed_color(
      const color<bpp>& color,
      const color_pair<bpp>& pair
   ) -> dual_color_name
   {
      if (color == pair.color0)
      {
         return dual_color_name::first;
      }
      else if (color == pair.color1)
      {
         return dual_color_name::second;
      }
      else {
         throw std::runtime_error("Color doesn't belong to pair. This shouldn't happen.");
      }
   }
}


template<int bpp>
auto bb::get_image(const std::string& filename) -> image<bpp>
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
auto bb::get_image_bytestream_naive(
   const image<bpp>& image
) -> std::vector<uint8_t>
{
   std::vector<uint8_t> result(image.get_byte_count());
   std::memcpy(result.data(), image.m_pixels.data(), image.get_byte_count());
   return result;
}


template<int bpp>
auto bb::get_image_bytestream_dual(
   const image<bpp>& image,
   const color_pair<bpp>& pair
) -> std::vector<uint8_t>
{
   const std::vector<dual_color_name> indexed_image = get_indexed_dual_image(image, pair);
   return get_bit_encoded(indexed_image, dual_color_name::second);
}


namespace bb::detail {
   template<int bpp>
   auto apply(
      const image<bpp>& image,
      const dual_image_type<bpp>& meta
   ) -> std::vector<uint8_t>
   {
      //return {};
      const color_pair<bpp> pair{ meta.color0, meta.color1 };
      return get_image_bytestream_dual(image, pair);
   }

   template<int bpp, int dual_bpp>
   auto apply(
      const image<bpp>&,
      const dual_image_type<dual_bpp>&
   ) -> std::vector<uint8_t>
   {
      // This should never happen
      std::terminate();
   }

   template<int bpp>
   auto apply(
      const image<bpp>& image,
      const naive_image_type&
   ) -> std::vector<uint8_t>
   {
      return get_image_bytestream_naive(image);
   }

   template<int bpp>
   auto apply(
      const image<bpp>&,
      const generic_binary&
   ) -> std::vector<uint8_t>
   {
      // This should never happen
      std::terminate();
   }
}


template<int bpp>
auto bb::get_image_bytestream(
   const image<bpp>& image,
   const content_meta& meta
) -> std::vector<uint8_t>
{
   return std::visit(
      [&](const auto& alternative) {
         return detail::apply(image, alternative);
      }
      , meta
   );
}


template<int bpp>
auto bb::get_color_pair(const image<bpp>& image) -> std::optional<color_pair<bpp>>
{
   if (image.get_pixel_count() == 0)
   {
      return std::nullopt;
   }
   else if (image.get_pixel_count() == 1)
   {
      return color_pair<bpp>{ image[0], image[0] };
   }
   else if (image.get_pixel_count() == 2)
   {
      return color_pair<bpp>{ image[0], image[1] };
   }
   else
   {
      return detail::get_actual_color_pair(image);
   }
}



template<int bpp>
auto bb::get_indexed_dual_image(
   const image<bpp>& im,
   const color_pair<bpp>& pair
) -> std::vector<dual_color_name>
{
   std::vector<dual_color_name> result;
   result.reserve(im.get_pixel_count());
   for (const auto& color : im)
   {
      result.emplace_back(detail::get_indexed_color(color, pair));
   }
   return result;;
}
