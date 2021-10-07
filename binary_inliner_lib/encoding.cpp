#include "encoding.h"

#include <format>
#include <fstream>
#include <optional>
#include <iostream>

#include "tools.h"
#include "file_tools.h"
#include "image.h"
#include "binary_image_tools.h"

#include "stb_image.h"


namespace {

   using namespace inliner;

   [[nodiscard]] auto is_image_path(const std::string& filename) -> bool
   {
      if (filename.ends_with("png"))
      {
         return true;
      }
      return false;
   }


   [[nodiscard]] auto get_binary_file_payload(const char* path) -> payload
   {
      const std::vector<uint8_t> file_content = get_binary_file(path);
      return { file_content, generic_binary{} };
   }


   [[nodiscard]] auto get_image_payload(const std::string& filename) -> payload
   {
      int width, height, components;
      unsigned char* data = stbi_load(filename.c_str(), &width, &height, &components, 0);
      if (data == NULL) {
         const std::string msg = std::format("Couldn't open file {}", filename);
         throw std::runtime_error(msg);
      }

      const payload result = [&]() {
         switch (components) {
         case 1:
            return detail::get_image_payload<1>(width, height, data);
         case 2:
            return detail::get_image_payload<2>(width, height, data);
         case 3:
            return detail::get_image_payload<3>(width, height, data);
         case 4:
            return detail::get_image_payload<4>(width, height, data);
         default:
            std::cout << "unexpected\n";
            return payload{};
         };
      }();

      stbi_image_free(data);
      return result;
   }

}


auto inliner::get_payload(const std::string& filename) -> payload
{
   if (is_image_path(filename))
      return get_image_payload(filename);
   else
      return get_binary_file_payload(filename.c_str());
}


auto inliner::write_payload(
   const std::string& filename,
   const payload& pl
) -> void
{
   
   const uint32_t content_size = static_cast<uint32_t>(pl.m_content_data.size());
   std::vector<uint8_t> target_bytes; // TODO reserve
   for (const uint8_t byte : meta_and_size_to_binary(pl, content_size))
      target_bytes.emplace_back(byte);
   append_copy(target_bytes, pl.m_content_data);

   std::ofstream filestream(filename, std::ios::out);
   if (!filestream.good())
   {
      std::cout << std::format("Couldn't open {} for writing\n", filename);
      return;
   }
   std::string content;
   // TODO think about good reserve estimate
   const int symbol_count = get_symbol_count<uint64_t>(static_cast<int>(target_bytes.size()));
   const uint64_t* ui64_ptr = reinterpret_cast<const uint64_t*>(target_bytes.data());
   for (int i = 0; i < symbol_count; ++i)
   {
      if (i > 0)
      {
         content += ", ";
      }
      content += get_ui64_str(ui64_ptr[i]);
   }
   filestream << std::format("constexpr uint64_t input[]{{\n    {}\n}};\n", content);
}


auto inliner::get_ui64_str(const uint64_t value) -> std::string
{
   return std::format("{:#018x}", value);
}
