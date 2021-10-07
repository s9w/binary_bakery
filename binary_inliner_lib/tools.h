#pragma once

#include <vector>

namespace inliner {

   struct no_init {};

   template<typename color_type>
   [[nodiscard]] constexpr auto get_symbol_count(const int byte_count) -> int;

   template<typename T>
   concept numerical = (std::integral<T> && !std::is_same_v<T, bool>) || std::floating_point<T>;

   template<inliner::numerical T>
   constexpr auto abs(const T value) noexcept -> T;

   template<inliner::numerical T>
   constexpr auto equal(const T a, const T b) noexcept -> bool;

   template <class T>
   auto append_copy(std::vector<T>& dst, std::vector<T>& src) -> void;

   template<typename ... Ts>
   auto get_byte_sequence(const Ts... value) -> std::vector<uint8_t>;

   struct binary_sequencer {
      std::vector<uint8_t> m_sequence;

      explicit binary_sequencer(const int reserve_size);

      template<class T>
      auto add(const T value) -> void;
   };

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
   std::vector<T>& src
) -> void
{
   dst.insert(std::end(dst), std::cbegin(src), std::cend(src));
}


template<typename ... Ts>
auto inliner::get_byte_sequence(
   const Ts... value
) -> std::vector<uint8_t>
{
   binary_sequencer sequencer(0);
   (sequencer.add(value), ...);
   return sequencer.m_sequence;
}



template<class T>
auto inliner::binary_sequencer::add(const T value) -> void
{
   m_sequence.resize(m_sequence.size() + sizeof(T));
   uint8_t* target = &m_sequence[m_sequence.size() - sizeof(T)];
   std::memcpy(target, &value, sizeof(T));
}
