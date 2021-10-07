#pragma once

#include <cstdint>
#include <variant>
#include <vector>

#include "image.h"


namespace inliner {

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

   using content_meta = std::variant<generic_binary, naive_image_type, dual_image_type<1>, dual_image_type<2>, dual_image_type<3>, dual_image_type<4>>;

   struct payload {
      std::vector<uint8_t> m_content_data;
      content_meta meta;
   };

   [[nodiscard]] auto meta_to_binary(const payload& pl) -> std::vector<uint8_t>;



   // storing:
   // uint8, 1 byte for type. 0 for generic, 1 for naive image, 2 for dual image

   // generic:
   // nothing more

   // both image types:
   // uint8_t, 1 bytes for bpp
   // uint16_t, 2 bytes for width
   // uint16_t, 2 bytes for height

   // indexed type
   // bpp*1 byte for color0
   // bpp*1 byte for color1

}
