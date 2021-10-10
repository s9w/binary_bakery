#pragma once

#include <string>
#include <optional>
#include <filesystem>

namespace fs = std::filesystem;

#include "content_meta.h"
#include "image.h"


namespace bb {

   struct config;

   // Content bytestream + meta object
   struct payload {
      std::vector<uint8_t> m_content_data;
      content_meta m_meta;
      bit_count m_bit_count;
      fs::path m_path;

      // Making sure no one is left behind during init
      payload(std::vector<uint8_t>&& content, const content_meta& meta, const bit_count& bit_count, const fs::path& path)
         : m_content_data(std::move(content))
         , m_meta(meta)
         , m_bit_count(bit_count)
         , m_path(path)
      {
         
      }
   };

   // TODO maybe make this optional and deal with exception from file opening, parsing errors etc
   [[nodiscard]] auto get_payload(const fs::path& path) -> payload;

   auto write_payloads_to_file(
      const config& cfg,
      std::vector<payload>&& payloads
   ) -> void;
}


namespace bb::detail
{
   [[nodiscard]] auto get_final_bytestream(payload& pl) -> std::vector<uint8_t>;

   template<alternative_of<content_meta> meta_type>
   [[nodiscard]] auto get_content_bit_count(
      const meta_type& meta,
      const std::vector<uint8_t>& stream
   ) -> bit_count
   {
      if constexpr (dual_image_type_c<meta_type>)
      {
         const int pixel_count = meta.m_width * meta.m_height;
         return bit_count{ pixel_count };
      }
      else
      {
         const byte_count bytes{ stream.size() };
         return bytes.get_bit_count();
      }
   }

}
