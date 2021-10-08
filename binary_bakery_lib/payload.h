#pragma once

#include <string>
#include <optional>

#include "content_meta.h"
#include "image.h"


namespace bb {

   struct config;

   // Content bytestream + meta object
   struct payload {
      std::vector<uint8_t> m_content_data;
      content_meta meta;
      int bit_count;

      // Making sure no one is left behind during init
      payload(std::vector<uint8_t>&& p_content, const content_meta& p_meta, int p_bit_count)
         : m_content_data(std::move(p_content))
         , meta(p_meta)
         , bit_count(p_bit_count)
      {}
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

   // Ugly, but MSVC bug, see
   // https://developercommunity2.visualstudio.com/t/Unnecessary-namespace-needed-for-declara/1406720
   template<bb::alternative_of<bb::content_meta> meta_type>
   auto get_image_meta_bit_count(const meta_type meta) -> bool;
}


template<int bpp>
auto bb::detail::get_image_payload(
   const int width,
   const int height,
   const unsigned char* image_data_ptr
) -> payload
{
   const image<bpp> image{ width, height, image_data_ptr };
   content_meta meta = get_image_meta(image);
   std::vector<uint8_t> stream = get_image_bytestream(image, meta);

   const int bit_count = std::visit([](const auto& alt) {return get_image_meta_bit_count(alt); }, meta);

   return { std::move(stream), std::move(meta), bit_count };
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


template<bb::alternative_of<bb::content_meta> meta_type>
auto bb::detail::get_image_meta_bit_count(const meta_type meta) -> bool
{
   if constexpr (dual_image_type_c<meta_type>)
   {
      const int pixel_count = meta.width * meta.height;
      return pixel_count;
   }
   else if constexpr (std::same_as<meta_type, naive_image_type>)
   {
      const int pixel_count = meta.width * meta.height;
      const int byte_count = pixel_count * meta.m_bpp;
      return byte_count * 8;
   }
   else
   {
      std::terminate();
   }
}
