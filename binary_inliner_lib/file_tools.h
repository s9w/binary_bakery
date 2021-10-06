#pragma once

#include <vector>

#include "tools.h"


namespace inliner
{

   struct binary_file_content {
      std::vector<uint64_t> data;
      int byte_count;
   };

   template<typename ... Ts>
   auto get_byte_sequence(const Ts... value) -> std::vector<uint8_t>;

   [[nodiscard]] auto get_binary_file(const char* path) -> binary_file_content;
   auto write_binary_file(const char* path, const std::vector<uint8_t>& byte_sequence) -> void;

}


namespace inliner::detail
{
   template<typename T>
   auto add_to_byte_sequence(
      std::vector<uint8_t>& sequence,
      const T value
   ) -> void;
}


template<typename T>
auto inliner::detail::add_to_byte_sequence(
   std::vector<uint8_t>& sequence,
   const T value
) -> void
{
   std::vector<uint8_t> value_sequence(sizeof(T));
   std::memcpy(value_sequence.data(), &value, sizeof(T));
   append_copy(sequence, value_sequence);
}


template<typename ... Ts>
auto inliner::get_byte_sequence(
   const Ts... value
) -> std::vector<uint8_t>
{
   std::vector<uint8_t> result;
   (detail::add_to_byte_sequence(result, value), ...);
   return result;
}


