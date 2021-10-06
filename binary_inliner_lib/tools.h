#pragma once

namespace inliner {

   template<typename color_type>
   [[nodiscard]] constexpr auto get_symbol_count(const int byte_count) -> int;
}



template<typename color_type>
constexpr auto inliner::get_symbol_count(const int byte_count) -> int
{
   const int complete_symbols = byte_count / sizeof(color_type);
   const int leftover_bytes = byte_count % sizeof(color_type);

   int symbol_count = complete_symbols;
   if (leftover_bytes > 0)
      ++symbol_count;
   return symbol_count;
}
