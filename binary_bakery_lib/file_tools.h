#pragma once

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;


namespace bb
{

   struct path_type {
   protected:
      fs::path m_path;

   public:
      explicit path_type(const fs::path& path);

      [[nodiscard]] auto get_path() const -> fs::path;
   };

   // Stores absolute file path. Ensures file exists. Can convert from relative path.
   struct abs_file_path : path_type {
      using path_type::path_type;

      explicit abs_file_path(const fs::path& path);
   };


   // Stores absolute directory path. Ensures directory exists. Can convert from relative path.
   struct abs_directory_path : path_type {
      using path_type::path_type;

      explicit abs_directory_path(const fs::path& path);
   };

   [[nodiscard]] auto get_binary_file(const abs_file_path& file) -> std::vector<uint8_t>;
   auto write_binary_file(const abs_file_path& file, const std::vector<uint8_t>& byte_sequence) -> void;

}
