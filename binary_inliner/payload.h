#pragma once

#include <cstdint>
#include <variant>
#include <vector>

#include "color.h"


namespace inliner {

   struct binary_type {};
   struct naive_image_type {
      int width = 0;
      int height = 0;
      int bpp = 0;
   };

   // This implies 1 channel image
   template<int bpp>
   struct dual_image_type {
      int width = 0;
      int height = 0;
      color<bpp> color0;
      color<bpp> color1;
   };
   static_assert(std::is_aggregate_v<dual_image_type<1>>);
   using content_meta = std::variant<binary_type, naive_image_type, dual_image_type<1>, dual_image_type<2>, dual_image_type<3>, dual_image_type<4>>;

   struct payload {
      std::vector<uint64_t> data;
      int byte_count;
      content_meta meta;
   };

}
