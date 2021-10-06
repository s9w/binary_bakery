#pragma once

#include <string>
#include <format>
#include <optional>

#include "payload.h"
#include "image.h"


namespace inliner {

   [[nodiscard]] auto encode(const std::string& filename) -> payload;

}

namespace inliner::detail{

   template<int bpp>
   [[nodiscard]] auto get_payload(
      const int width,
      const int height,
      const unsigned char* image_data_ptr
   )->payload;


   template<int bpp>
   [[nodiscard]] auto get_image_meta(
      const image<bpp>& image
   ) -> content_meta;

}


template<int bpp>
auto inliner::detail::get_payload(
   const int width,
   const int height,
   const unsigned char* image_data_ptr
) -> payload
{
   const image<bpp> image{ width, height, image_data_ptr };
   const content_meta meta = get_image_meta(image);
   return { image.to_uint64(), image.get_byte_count(), meta };
}


template<int bpp>
auto inliner::detail::get_image_meta(
   const image<bpp>& image
) -> content_meta
{
   const std::optional<color_pair<bpp>> color_pair = get_color_pair<bpp>(image);
   if (color_pair.has_value())
   {
      return dual_image_type<bpp>{
         image.m_width,
            image.m_height,
            color_pair.value().color0,
            color_pair.value().color1
      };
   }
   else
   {
      return naive_image_type{ image.m_width, image.m_height, bpp };
   }
}
