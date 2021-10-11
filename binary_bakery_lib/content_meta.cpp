#include "content_meta.h"

#include "tools.h"

#include "../binary_bakery_decoder.h"

namespace
{
   using namespace bb;


   template<typename target_type, typename meta_type>
   [[nodiscard]] constexpr auto get_bpp(
      const meta_type& meta
   ) -> target_type
   {
      if constexpr (std::same_as<meta_type, naive_image_type>)
      {
         return static_cast<target_type>(meta.m_bpp);
      }
      else
      {
         return target_type{};
      }
   }


   template<typename meta_type>
   [[nodiscard]] constexpr auto get_width(
      const meta_type& meta
   ) -> uint16_t
   {
      if constexpr (std::same_as<meta_type, naive_image_type>)
         return static_cast<uint16_t>(meta.m_width);
      else
         return 0ui16;
   }


   template<typename meta_type>
   [[nodiscard]] constexpr auto get_height(
      const meta_type& meta
   ) -> uint16_t
   {
      if constexpr (std::same_as<meta_type, naive_image_type>)
         return static_cast<uint16_t>(meta.m_height);
      else
         return 0ui16;
   }


   [[nodiscard]] constexpr auto get_compression_int(
      const compression_mode compression
   ) -> uint8_t
   {
      switch (compression) {
      case compression_mode::none:
         return 0ui8;
         break;
      case compression_mode::zstd:
         return 1ui8;
         break;
      case compression_mode::lz4:
         return 2ui8;
         break;
      default:
         std::terminate();
         break;
      }
   }

} // namespace {}


auto bb::meta_and_size_to_header_stream(
   const content_meta& meta,
   const compression_mode compression,
   const byte_count uncompressed_size,
   const byte_count compressed_size
) -> std::array<uint8_t, 16>
{
   header head;
   head.type = static_cast<uint8_t>(meta.index());
   head.compression = get_compression_int(compression);
   head.version = 0ui8;
   head.bpp = std::visit(
      [&](const auto alternative) {return get_bpp<uint8_t>(alternative); }
      , meta
   );
   head.width = std::visit([&](const auto alternative) {return ::get_width(alternative); }, meta);
   head.height = std::visit([&](const auto alternative) {return ::get_height(alternative); }, meta);
   head.decompressed_size = static_cast<uint32_t>(uncompressed_size.m_value);
   head.compressed_size = static_cast<uint32_t>(compressed_size.m_value);

   return std::bit_cast<std::array<uint8_t, 16>>(head);
}

