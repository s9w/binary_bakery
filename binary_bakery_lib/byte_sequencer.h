#pragma once

#include <array>
#include <exception>
#include <vector>

#include "universal.h"


namespace bb
{

   // Encodes values into a uint8-array until its target size is reached. Will complain if that's
   // not the case.
   template<byte_count bytes>
   struct binary_sequencer {
   private:
      using array_type = std::array<uint8_t, bytes.m_value>;
      array_type m_sequence{};
      int m_position = 0;

   public:
      explicit constexpr binary_sequencer() = default;
      constexpr ~binary_sequencer();

      template<typename T>
      constexpr auto add(const T value) -> void;

      template<typename ... Ts>
      constexpr auto add(const Ts... values) -> void;

      [[nodiscard]] constexpr auto get() const -> const array_type&;

      binary_sequencer(const binary_sequencer&) = delete;
      binary_sequencer& operator=(const binary_sequencer&) = delete;
      binary_sequencer(binary_sequencer&&) = delete;
      binary_sequencer& operator=(binary_sequencer&&) = delete;
   };
   using header_sequencer = binary_sequencer<byte_count{24}>;

   template<typename ... Ts>
   [[nodiscard]] auto get_byte_sequence(const Ts... value) -> std::vector<uint8_t>;
   
} // namespace bb


template<typename ... Ts>
auto bb::get_byte_sequence(
   const Ts... value
) -> std::vector<uint8_t>
{
   binary_sequencer<byte_sizeof<Ts...>> sequencer;
   sequencer.add(value...);

   std::vector<uint8_t> result;
   result.reserve(std::size(sequencer.get()));
   for (const uint8_t byte : sequencer.get())
      result.emplace_back(byte);
   return result;
}


template <bb::byte_count bytes>
constexpr bb::binary_sequencer<bytes>::~binary_sequencer()
{
   if (m_position != bytes.m_value)
   {
      std::terminate();
   }
}


template<bb::byte_count bytes>
template<typename T>
constexpr auto bb::binary_sequencer<bytes>::add(const T value) -> void
{
   constexpr int chunk_size = sizeof(T);
   using chunk_type = std::array<uint8_t, chunk_size>;
   const auto chunk = std::bit_cast<chunk_type>(value);
   for (int i = 0; i < chunk_size; ++i)
   {
      m_sequence[m_position] = chunk[i];
      ++m_position;
   }
}


template<bb::byte_count bytes>
template<typename ... Ts>
constexpr auto bb::binary_sequencer<bytes>::add(const Ts... values) -> void
{
   (add(values), ...);
}


template <bb::byte_count bytes>
constexpr auto bb::binary_sequencer<bytes>::get() const -> const array_type&
{
   if (m_position != bytes.m_value)
   {
      std::terminate();
   }
   return m_sequence;
}
