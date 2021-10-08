#pragma once

#include <cstdint>
#include <variant>
#include <vector>

#include "color.h"


namespace bb {

   struct generic_binary {};

   struct naive_image_type {
      int width = 0;
      int height = 0;
      int m_bpp = 0;
   };

   // Image with only two unique colors
   template<int bpp>
   struct dual_image_type {
      int width = 0;
      int height = 0;
      int m_bpp = bpp;
      color<bpp> color0;
      color<bpp> color1;
   };

   template<typename T>
   concept dual_image_type_c = requires(T t) {
      t.color0;
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

   // Content bytestream + meta object
   struct payload {
      std::vector<uint8_t> m_content_data;
      content_meta meta;
   };

   [[nodiscard]] auto meta_and_size_to_binary(const payload& pl, const uint32_t data_bit_count) -> std::array<uint8_t, 24>;

}
