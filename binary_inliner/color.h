#pragma once

#include <compare>


namespace inliner {

   template<int bpp>
   struct color {
      uint8_t components[bpp];
      auto operator<=>(const color<bpp>&) const = default;
   };

   template<int bpp>
   struct image_view {
      using color_type = color<bpp>;

      int width = 0;
      int height = 0;
      color_type* m_pixel_ptr;

      image_view(const int w, const int h, unsigned char* data)
         : width(w)
         , height(h)
         , m_pixel_ptr(std::bit_cast<color_type*>(data))
      {}

      [[nodiscard]] auto operator[](const int index) const -> const color_type& {
         return m_pixel_ptr[index];
      }

      auto begin() const -> color_type* {
         return &m_pixel_ptr[0];
      }
      auto end() const -> color_type* {
         const int pixel_count = width * height;
         return &m_pixel_ptr[pixel_count-1];
      }
   };

}
