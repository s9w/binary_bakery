#pragma once

#include <iterator> // std::back_inserter
#include <string>
#include <vector>

#include "universal.h"

namespace bb {

   template<typename T>
   [[nodiscard]] constexpr auto get_symbol_count(const byte_count count) -> int;

   [[nodiscard]] auto get_replaced_str(const std::string& source, const std::string& from, const std::string& to) -> std::string;

   template <class T>
   auto append_moved(std::vector<T>& dst, std::vector<T>&& src) -> void;

   auto append_ui64_str(const uint64_t value, std::string& target) -> void;

   [[nodiscard]] auto get_human_readable_size(const byte_count bytes) -> std::string;

}


template<typename T>
constexpr auto bb::get_symbol_count(const byte_count count) -> int
{
   const auto [complete_symbols, leftover_bytes] = std::div(count.m_value, sizeof(T));

   if (leftover_bytes == 0) return complete_symbols;
   else                     return complete_symbols + 1;
}


template <class T>
void bb::append_moved(std::vector<T>& dst, std::vector<T>&& src)
{
   if (dst.empty())
      dst = std::move(src);
   else {
      dst.reserve(dst.size() + src.size());
      std::move(std::begin(src), std::end(src), std::back_inserter(dst));
      src.clear();
   }
}
