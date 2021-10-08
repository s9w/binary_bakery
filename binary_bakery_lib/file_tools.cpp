#include "file_tools.h"

#include <fstream>
#include <format>


auto bb::write_binary_file(
   const char* path,
   const std::vector<uint8_t>& byte_sequence
) -> void
{
   std::ofstream file(path, std::ios::out | std::ios::binary);
   file.write(reinterpret_cast<const char*>(byte_sequence.data()), byte_sequence.size());
}



auto bb::get_binary_file(const char* path) -> std::vector<uint8_t>
{
   std::ifstream file(path, std::ios::ate | std::ios::binary);

   if (file.is_open() == false) {
      const std::string msg = std::format("Couldn't open file {}", path);
      throw std::runtime_error(msg);
   }

   const size_t byte_count = file.tellg();
   std::vector<uint8_t> buffer(byte_count);

   file.seekg(0);
   file.read(reinterpret_cast<char*>(buffer.data()), byte_count);
   return buffer;
}
