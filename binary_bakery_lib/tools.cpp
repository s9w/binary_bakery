#include "tools.h"

#include <format>


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


auto bb::get_ui64_str(const uint64_t value) -> std::string
{
   return std::format("{:#018x}", value);
}
