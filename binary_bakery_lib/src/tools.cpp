#include <binary_bakery_lib/tools.h>

#include <stb_image.h>
#include <fmt/format.h>


auto bb::get_replaced_str(
   const std::string& source,
   const std::string& from,
   const std::string& to
) -> std::string
{
   if (from.empty())
      return source;
   std::string result;
   result.reserve(source.length());

   std::string::size_type source_complete_cursor = 0;

   for (auto find_pos = source.find(from, source_complete_cursor);
      find_pos != std::string::npos;
      find_pos = source.find(from, source_complete_cursor))
   {
      const std::string::size_type append_count = find_pos - source_complete_cursor;
      result.append(source, source_complete_cursor, append_count);
      result += to;
      source_complete_cursor = find_pos + from.length();
   }
   result += source.substr(source_complete_cursor);
   return result;
}


auto bb::append_ui64_str(
   const uint64_t value,
   std::string& target
) -> void
{
   target += "0x";
   for (int i = 0; i < 16; ++i)
   {
      const int shift_amount = 60 - 4 * i;
      const uint64_t mask = static_cast<uint64_t>(15) << shift_amount;
      const uint8_t part = static_cast<uint8_t>((value & mask) >> shift_amount);
      if (part < 10)
         target += static_cast<char>('0' + part);
      else
         target += static_cast<char>('a' + part - 10);
   }
}


auto bb::get_human_readable_size(const byte_count bytes) -> std::string
{
   const double kb = bytes.m_value / 1024.0;
   const double mb = bytes.m_value / static_cast<double>(1024 * 1024);
   if(mb >= 1.0)
   {
      return fmt::format("{:.2f} MB", mb);
   }
   else if(kb >= 1.0)
   {
      return fmt::format("{:.2f} KB", kb);
   }
   else
   {
      return fmt::format("{} bytes", bytes.m_value);
   }
}
