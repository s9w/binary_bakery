#pragma once

#include <vector>

#include <binary_bakery_lib/content_meta.h>
#include <binary_bakery_lib/file_tools.h>


namespace bb {

   struct config;

   // Content bytestream + meta object
   struct payload {
      std::vector<uint8_t> m_content_data;
      content_meta m_meta;
      abs_file_path m_path;

      // Making sure no one is left behind during init
      payload(std::vector<uint8_t>&& content, const content_meta& meta, const abs_file_path& file)
         : m_content_data(std::move(content))
         , m_meta(meta)
         , m_path(file)
      {
         
      }
   };

   // TODO maybe make this optional and deal with exception from file opening, parsing errors etc
   [[nodiscard]] auto get_payload(const abs_file_path& path, const config& cfg) -> payload;

   auto write_payloads_to_file(
      const config& cfg,
      std::vector<payload>&& payloads,
      const abs_directory_path& working_dir
   ) -> void;
}


namespace bb::detail
{
   [[nodiscard]] auto get_final_bytestream(payload& pl, const config& cfg) -> std::vector<uint8_t>;

}
