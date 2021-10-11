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
      fs::path m_path;

      // Making sure no one is left behind during init
      payload(std::vector<uint8_t>&& content, const content_meta& meta, const fs::path& path)
         : m_content_data(std::move(content))
         , m_meta(meta)
         , m_path(path)
      {
         
      }
   };

   // TODO maybe make this optional and deal with exception from file opening, parsing errors etc
   [[nodiscard]] auto get_payload(const fs::path& path) -> payload;

   auto write_payloads_to_file(
      const config& cfg,
      std::vector<payload>&& payloads,
      const fs::path& working_dir
   ) -> void;
}


namespace bb::detail
{
   [[nodiscard]] auto get_final_bytestream(payload& pl, const config& cfg) -> std::vector<uint8_t>;

}
