#pragma once

#include <string>
#include <format>
#include <optional>

#include "content_meta.h"
#include "image.h"


namespace bb {

   struct config;

   // Content bytestream + meta object
   struct payload {
      std::vector<uint8_t> m_content_data;
      content_meta meta;
   };

   [[nodiscard]] auto get_payload(const std::string& filename) -> payload;

   auto write_payload_to_file(
      const std::string& filename,
      const std::string& variable_name,
      const config& cfg,
      const payload& pl
   ) -> void;
}

namespace bb::detail{

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
auto bb::detail::get_image_payload(
   const int width,
   const int height,
   const unsigned char* image_data_ptr
) -> payload
{
   const image<bpp> image{ width, height, image_data_ptr };
   const content_meta meta = get_image_meta(image);
   return { get_image_bytestream(image, meta), meta };
}


template<int bpp>
auto bb::detail::get_image_meta(
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
