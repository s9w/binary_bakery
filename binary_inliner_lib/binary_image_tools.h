#pragma once

#include <optional>
#include <vector>

#include "image.h"



namespace inliner
{

   template<int bpp>
   struct color_pair {
      color<bpp> color0;
      color<bpp> color1;
   };


   template<int bpp>
   [[nodiscard]] auto get_color_pair(
      const image<bpp>& image
   ) -> std::optional<color_pair<bpp>>;

   enum class color_name{first, second};

   template<int bpp>
   [[nodiscard]] auto get_indexed_dual_image(
      unsigned char* data,
      const int width,
      const int height,
      const color_pair<bpp>& pair
   ) -> std::vector<color_name>;

}


