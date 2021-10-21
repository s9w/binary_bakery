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

   // Stores absolute file path. Throws if file doesn't exist. Can convert from relative path.
   struct abs_file_path : path_type {
      using path_type::path_type;

      explicit abs_file_path(const fs::path& path);
   };


   // Stores absolute directory path. Throws if path doesn't exist. Can convert from relative path.
   struct abs_directory_path : path_type{
      using path_type::path_type;

      explicit abs_directory_path(const fs::path& path);
   };

   [[nodiscard]] auto get_binary_file(const abs_file_path& file) -> std::vector<uint8_t>;

}
