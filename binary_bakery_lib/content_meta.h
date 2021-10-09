#pragma once

#include <cstdint>
#include <variant>

#include "color.h"
#include "universal.h"


namespace bb {

   struct generic_binary {};

   struct naive_image_type {
      int m_width = 0;
      int m_height = 0;
      int m_bpp = 0;
   };

   // Image with only two unique colors
   template<int bpp>
   struct dual_image_type {
      static constexpr int m_bpp = bpp;

      int m_width = 0;
      int m_height = 0;
      color<bpp> m_color0;
      color<bpp> m_color1;
   };

   template<typename T>
   concept dual_image_type_c = requires(T t) {
      t.m_color0;
   };

   template<typename T>
   concept image_type_c = requires(T t) {
      t.m_bpp;
   };

   using content_meta = std::variant<
      generic_binary,
      naive_image_type,
      dual_image_type<1>, dual_image_type<2>, dual_image_type<3>, dual_image_type<4>
   >;

   [[nodiscard]] auto meta_and_size_to_header_stream(
      const content_meta& meta,
      const bit_count data_bit_count
   ) -> std::array<uint8_t, 24>;

}
