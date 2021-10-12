#include "file_tools.h"

#include <fstream>
#include <format>

namespace
{

   auto get_absolute_path(const fs::path& path) -> fs::path
   {
      if (fs::exists(path) == false)
         throw std::exception("Path doesn't exist");
      if (path.is_absolute())
         return path;
      else
         return fs::absolute(path);
   }


   auto get_absolute_file_path(const fs::path& path) -> fs::path
   {
      const fs::path absolute = get_absolute_path(path);
      if (fs::is_regular_file(absolute) == false)
         throw std::exception("Path is no file");
      return absolute;
   }


   auto get_absolute_directory_path(const fs::path& path) -> fs::path
   {
      const fs::path absolute = get_absolute_path(path);
      if (fs::is_directory(absolute) == false)
         throw std::exception("Path is no directory");
      return absolute;
   }

} // namespace {}


auto bb::write_binary_file(
   const abs_file_path& file_p,
   const std::vector<uint8_t>& byte_sequence
) -> void
{
   std::ofstream file(file_p.get_path(), std::ios::out | std::ios::binary);
   const auto byte_size = static_cast<std::streamsize>(byte_sequence.size());
   file.write(reinterpret_cast<const char*>(byte_sequence.data()), byte_size);
}



auto bb::get_binary_file(const abs_file_path& file_p) -> std::vector<uint8_t>
{
   std::ifstream file(file_p.get_path(), std::ios::ate | std::ios::binary);

   if (file.is_open() == false) {
      const std::string msg = std::format("Couldn't open file {}", file_p.get_path().string());
      throw std::runtime_error(msg);
   }

   const size_t byte_count = file.tellg();
   std::vector<uint8_t> buffer(byte_count);

   file.seekg(0);
   file.read(
      reinterpret_cast<char*>(buffer.data()),
      static_cast<std::streamsize>(byte_count)
   );
   return buffer;
}


bb::path_type::path_type(const fs::path& path)
   : m_path(path)
{

}


bb::abs_file_path::abs_file_path(const fs::path& path)
   : path_type(get_absolute_file_path(path))
{

}


bb::abs_directory_path::abs_directory_path(const fs::path& path)
   : path_type(get_absolute_directory_path(path))
{

}


auto bb::path_type::get_path() const -> fs::path
{
    return m_path;
}
