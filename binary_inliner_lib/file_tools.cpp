#include "file_tools.h"

#include <fstream>
#include <format>


auto inliner::write_binary_file(
   const char* path,
   const std::vector<uint8_t>& byte_sequence
) -> void
{
   std::ofstream file(path, std::ios::out | std::ios::binary);
   file.write(reinterpret_cast<const char*>(byte_sequence.data()), byte_sequence.size());
}



auto inliner::get_binary_file(const char* path) -> binary_file_content
{
   std::ifstream file(path, std::ios::ate | std::ios::binary);

   if (file.is_open() == false) {
      const std::string msg = std::format("Couldn't open file {}", path);
      throw std::runtime_error(msg);
   }

   const size_t byte_count = file.tellg();
   const int symbol_count = get_symbol_count<uint64_t>(static_cast<int>(byte_count));
   std::vector<uint64_t> buffer(symbol_count);

   file.seekg(0);
   file.read(reinterpret_cast<char*>(buffer.data()), byte_count);
   return { buffer, static_cast<int>(byte_count) };
}
