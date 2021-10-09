#pragma once

#include <vector>
#include <array>
#include <iterator>
#include <string>
#include <variant>

#include "universal.h"

namespace bb {

   template<typename color_type>
   [[nodiscard]] constexpr auto get_symbol_count(const byte_count count) -> int;

   [[nodiscard]] auto get_replaced_str(const std::string& source, const std::string& from, const std::string& to) -> std::string;

   template <class T>
   auto append_copy(std::vector<T>& dst, const std::vector<T>& src) -> void;

   template <class T>
   auto append_moved(std::vector<T>& dst, std::vector<T>& src) -> void;

   [[nodiscard]] auto write_ui64_str(const uint64_t value, std::string& target) -> void;

   template<typename enum_type>
   [[nodiscard]] auto get_bit_encoded(const std::vector<enum_type>& enums, const enum_type one_value) -> std::vector<uint8_t>;
}



template<typename color_type>
constexpr auto bb::get_symbol_count(const byte_count count) -> int
{
   const int complete_symbols = count.m_value / sizeof(color_type);
   const int leftover_bytes = count.m_value % sizeof(color_type);

   int symbol_count = complete_symbols;
   if (leftover_bytes > 0)
      ++symbol_count;
   return symbol_count;
}


template <class T>
auto bb::append_copy(
   std::vector<T>& dst,
   const std::vector<T>& src
) -> void
{
   dst.insert(std::end(dst), std::cbegin(src), std::cend(src));
}


template <class T>
void bb::append_moved(std::vector<T>& dst, std::vector<T>& src)
{
   if (dst.empty())
      dst = std::move(src);
   else {
      dst.reserve(dst.size() + src.size());
      std::move(std::begin(src), std::end(src), std::back_inserter(dst));
      src.clear();
   }
}


template<typename enum_type>
auto bb::get_bit_encoded(
   const std::vector<enum_type>& enums,
   const enum_type one_value
) -> std::vector<uint8_t>
{
   const size_t bit_count = enums.size();
   size_t byte_count = bit_count / 8;
   if ((bit_count % 8) > 0)
      ++byte_count;

   // streaming from left to right
   std::vector<uint8_t> result(byte_count, 0ui8);
   for (int i = 0; i < bit_count; ++i)
   {
      const auto [byte_index, bit_index] = std::div(i, 8);
      const bool binary_value = enums[i] == one_value;
      const int left_shift_amount = 7 - bit_index; // 7 is leftest bit
      const uint8_t new_bits = binary_value << left_shift_amount;
      result[byte_index] = result[byte_index] | new_bits;
   }
   return result;
}
