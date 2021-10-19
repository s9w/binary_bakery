#include <binary_bakery_lib/file_tools.h>

#include <fstream>

#include <fmt/format.h>


namespace
{

   auto get_absolute_path(const fs::path& path) -> fs::path
   {
      if (fs::exists(path) == false)
         throw std::runtime_error("Path doesn't exist");
      if (path.is_absolute())
         return path;
      else
         return fs::absolute(path);
   }


   auto get_absolute_file_path(const fs::path& path) -> fs::path
   {
      const fs::path absolute = get_absolute_path(path);
      if (fs::is_regular_file(absolute) == false)
         throw std::runtime_error("Path is no file");
      return absolute;
   }


   auto get_absolute_directory_path(const fs::path& path) -> fs::path
   {
      const fs::path absolute = get_absolute_path(path);
      if (fs::is_directory(absolute) == false)
         throw std::runtime_error("Path is no directory");
      return absolute;
   }

} // namespace {}



auto bb::get_binary_file(const abs_file_path& file_p) -> std::vector<uint8_t>
{
   std::ifstream file(file_p.get_path(), std::ios::ate | std::ios::binary);

   if (file.is_open() == false) {
      const std::string msg = fmt::format("Couldn't open file {}", file_p.get_path().string());
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
