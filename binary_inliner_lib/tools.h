#pragma once

#include <vector>
#include <array>
#include <iterator>

namespace inliner {

   struct no_init {};

   template<typename color_type>
   [[nodiscard]] constexpr auto get_symbol_count(const int byte_count) -> int;

   template<typename T>
   concept numerical = (std::integral<T> && !std::is_same_v<T, bool>) || std::floating_point<T>;

   template<inliner::numerical T>
   [[nodiscard]] constexpr auto abs(const T value) noexcept -> T;

   template<inliner::numerical T>
   [[nodiscard]] constexpr auto equal(const T a, const T b) noexcept -> bool;

   template <class T>
   auto append_copy(std::vector<T>& dst, const std::vector<T>& src) -> void;

   template <class T>
   auto append_moved(std::vector<T>& dst, std::vector<T>& src) -> void;

   template<typename ... Ts>
   [[nodiscard]] auto get_byte_sequence(const Ts... value) -> std::vector<uint8_t>;

   template<typename enum_type>
   [[nodiscard]] auto get_bit_encoded(const std::vector<enum_type>& enums, const enum_type one_value) -> std::vector<uint8_t>;

   // TODO this only needs to live in user-facing decoder.h
   template<int bit_count, typename T>
   [[nodiscard]] auto get_bit_decoded(const std::vector<uint8_t>& enums, const T& value0, const T& value1) -> std::array<T, bit_count>;

   template<int byte_count>
   struct binary_sequencer {
      std::array<uint8_t, byte_count> m_sequence;
      int m_position = 0;

      explicit binary_sequencer() = default;
      ~binary_sequencer() {
         const int target_position = byte_count;
         if (m_position != target_position) {
            std::terminate();
         }
      }

      template<class T>
      auto add(const T value) -> void;
   };
   using header_sequencer = binary_sequencer<24>;

   template<typename... Ts>
   constexpr unsigned int actual_sizeof = (0 + ... + sizeof(Ts));
   static_assert(actual_sizeof<double, int> == 12);

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


template<inliner::numerical T>
constexpr auto inliner::abs(const T value) noexcept -> T
{
   if constexpr (std::is_unsigned_v<T>)
      return value;
   else
      return value < T{} ? -value : value;
}


template<inliner::numerical T>
constexpr auto inliner::equal(
   const T a,
   const T b
) noexcept -> bool
{
   constexpr double tol = 0.001;
   if constexpr (std::is_integral_v<T>)
      return a == b;
   else
      return inliner::abs(a - b) <= tol;
}


template <class T>
auto inliner::append_copy(
   std::vector<T>& dst,
   const std::vector<T>& src
) -> void
{
   dst.insert(std::end(dst), std::cbegin(src), std::cend(src));
}


template <class T>
void inliner::append_moved(std::vector<T>& dst, std::vector<T>& src)
{
   if (dst.empty())
      dst = std::move(src);
   else {
      dst.reserve(dst.size() + src.size());
      std::move(std::begin(src), std::end(src), std::back_inserter(dst));
      src.clear();
   }
}


template<typename ... Ts>
auto inliner::get_byte_sequence(
   const Ts... value
) -> std::vector<uint8_t>
{
   const int byte_count = actual_sizeof<Ts...>;
   binary_sequencer<byte_count> sequencer;
   (sequencer.add(value), ...);
   std::vector<uint8_t> result;

   result.reserve(byte_count);
   for (const uint8_t byte : sequencer.m_sequence)
      result.emplace_back(byte);
   return result;
}


template<int byte_count>
template<class T>
auto inliner::binary_sequencer<byte_count>::add(const T value) -> void
{
   uint8_t* target = &m_sequence[m_position];
   std::memcpy(target, &value, sizeof(T));
   m_position += sizeof(T);
}


template<typename enum_type>
auto inliner::get_bit_encoded(
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


template<int bit_count, typename T>
auto inliner::get_bit_decoded(
   const std::vector<uint8_t>& enums,
   const T& value0,
   const T& value1
) -> std::array<T, bit_count>
{
   std::array<T, bit_count> result;
   for (int i = 0; i < bit_count; ++i)
   {
      const auto [source_byte_index, source_bit_index] = std::div(i, 8);
      const int left_shift_amount = 7 - source_bit_index; // 7 is leftest bit
      const uint8_t mask = 1ui8 << left_shift_amount;
      const bool binary_value = static_cast<bool>((enums[source_byte_index] & mask) >> left_shift_amount);
      result[i] = binary_value ? value1 : value0;
   }
   return result;
}
