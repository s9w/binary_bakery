#pragma once

#include <string>
#include <format>
#include <optional>

#include "payload.h"
#include "image.h"


namespace inliner {

   [[nodiscard]] auto get_payload(const std::string& filename) -> payload;
   auto write_payload(const std::string& filename, const std::string& variable_name, const payload& pl) -> void;

   [[nodiscard]] auto get_ui64_str(const uint64_t value) -> std::string;

}

namespace inliner::detail{

   template<int bpp>
   [[nodiscard]] auto get_image_payload(
      const int width,
      const int height,
      const unsigned char* image_data_ptr
   ) -> payload;

   template<int bpp>
   [[nodiscard]] auto get_image_meta(const image<bpp>& image) -> content_meta;
}


template<int bpp>
auto inliner::detail::get_image_payload(
   const int width,
   const int height,
   const unsigned char* image_data_ptr
) -> payload
{
   const image<bpp> image{ width, height, image_data_ptr };
   const content_meta meta = get_image_meta(image);
   return { image.to_uint8(), meta };
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
         bpp,
         color_pair.value().color0,
         color_pair.value().color1
      };
   }
   else
   {
      return naive_image_type{ image.m_width, image.m_height, bpp };
   }
}
