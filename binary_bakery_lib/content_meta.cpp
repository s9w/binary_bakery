#include "content_meta.h"

#include "../binary_bakery_decoder.h" // for header type


namespace
{
   using namespace bb;

   template<typename accessor_type>
   [[nodiscard]] constexpr auto get_property(
      const content_meta& meta_var,
      const accessor_type& accessor
   ) -> int
   {
      if (std::holds_alternative<naive_image_type>(meta_var))
         return accessor(std::get<naive_image_type>(meta_var));
      else
         return 0;
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


   [[nodiscard]] auto get_type_index(
      const content_meta& meta
   ) -> int
   {
      if (std::holds_alternative<generic_binary>(meta))
         return 0;
      else if (std::holds_alternative<naive_image_type>(meta))
         return 1;
      else
         std::terminate();
   }

} // namespace {}


auto bb::get_header_bytes(
    const content_meta& meta,
    const compression_mode compression,
   const byte_count& uncompressed_size,
   const byte_count& compressed_size
) -> std::array<uint8_t, 16>
{
   header head;
   head.type = static_cast<uint8_t>(get_type_index(meta));
   head.compression = get_compression_int(compression);
   head.version = 0ui8;
   head.bpp = static_cast<uint8_t>(get_property(meta, [](const naive_image_type& image) {return image.m_bpp; }));
   head.width = static_cast<uint8_t>(get_property(meta, [](const naive_image_type& image) {return image.m_width; }));
   head.height = static_cast<uint8_t>(get_property(meta, [](const naive_image_type& image) {return image.m_height; }));

   head.decompressed_size = static_cast<uint32_t>(uncompressed_size.m_value);
   head.compressed_size = static_cast<uint32_t>(compressed_size.m_value);

   return std::bit_cast<std::array<uint8_t, 16>>(head);
}

