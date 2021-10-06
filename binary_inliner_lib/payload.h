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
      int bpp = 0;
   };

   // Image with only two unique colors
   template<int bpp>
   struct dual_image_type {
      int width = 0;
      int height = 0;
      color<bpp> color0;
      color<bpp> color1;
   };

   using content_meta = std::variant<generic_binary, naive_image_type, dual_image_type<1>, dual_image_type<2>, dual_image_type<3>, dual_image_type<4>>;

   struct payload {
      std::vector<uint64_t> data;
      int byte_count;
      content_meta meta;
   };

}
