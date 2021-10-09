#include "file_tools.h"

#include <fstream>
#include <format>


auto bb::write_binary_file(
   const fs::path& path,
   const std::vector<uint8_t>& byte_sequence
) -> void
{
   std::ofstream file(path, std::ios::out | std::ios::binary);
   const auto byte_size = static_cast<std::streamsize>(byte_sequence.size());
   file.write(reinterpret_cast<const char*>(byte_sequence.data()), byte_size);
}



auto bb::get_binary_file(const fs::path& path) -> std::vector<uint8_t>
{
   std::ifstream file(path, std::ios::ate | std::ios::binary);

   if (file.is_open() == false) {
      const std::string msg = std::format("Couldn't open file {}", path.string());
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
