#include "encoding.h"

#include <format>
#include <fstream>
#include <optional>
#include <iostream>

#include "tools.h"
#include "file_tools.h"
#include "image.h"

#include "stb_image.h"


namespace {

   using namespace bb;

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


   template<dual_image_type_c meta_type>
   [[nodiscard]] auto get_bit_count_impl(
      const meta_type& meta
   ) -> int
   {
      const int pixel_count = meta.width * meta.height;
      return pixel_count;
   }


   template<typename T>
   [[nodiscard]] auto get_bit_count_impl(
      const T&
   ) -> int
   {
      std::terminate();
   }

   
   [[nodiscard]] auto get_bit_count(
      const payload& pl
   ) -> int
   {
      if (std::holds_alternative<generic_binary>(pl.meta) || std::holds_alternative<naive_image_type>(pl.meta))
      {
         const int byte_size = static_cast<int>(pl.m_content_data.size());
         return byte_size * 8;
      }
      else
      {
         return std::visit(
            [](const auto& alternative) {
               return get_bit_count_impl(alternative);
            },
            //get_bit_count_impl,
            pl.meta
         );
      }
   }

} // namespace {}


auto bb::get_payload(const std::string& filename) -> payload
{
   if (is_image_path(filename))
      return get_image_payload(filename);
   else
      return get_binary_file_payload(filename.c_str());
}


auto bb::write_payload_to_file(
   const std::string& filename,
   const std::string& variable_name,
   const int indentation,
   const int max_columns,
   const payload& pl
) -> void
{
   const uint32_t bit_count = get_bit_count(pl); // if indexed then over dimensions, otherwise bytestream size

   std::vector<uint8_t> target_bytes; // TODO reserve
   for (const uint8_t byte : meta_and_size_to_binary(pl, bit_count))
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

   const int groups_per_line = (max_columns - indentation) / (2 + 16 + 2);
   int in_line_count = 0;
   const std::string indentation_str(indentation, ' ');
   for (int i = 0; i < symbol_count; ++i)
   {
      content += get_ui64_str(ui64_ptr[i]);
      if (i < (symbol_count-1))
      {
         content += ", ";
      }
      ++in_line_count;
      if(in_line_count == groups_per_line)
      {
         content += std::format("\n{}", indentation_str);
         in_line_count = 0;
      }
   }
   
   filestream << std::format("constexpr uint64_t {0}[]{{\n{1}{2}\n}};\n", variable_name, indentation_str, content);
}


auto bb::get_ui64_str(const uint64_t value) -> std::string
{
   return std::format("{:#018x}", value);
}
