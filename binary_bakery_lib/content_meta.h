#pragma once

#include <array>
#include <variant>

#include "universal.h"


namespace bb {

   struct generic_binary {};

   struct naive_image_type {
      int m_width = 0;
      int m_height = 0;
      int m_bpp = 0;
   };

   using content_meta = std::variant<generic_binary, naive_image_type>;

   [[nodiscard]] auto meta_and_size_to_header_stream(
      const content_meta& meta,
      const compression_mode compression,
      const byte_count uncompressed_size,
      const byte_count compressed_size
   ) -> std::array<uint8_t, 16>;

}
