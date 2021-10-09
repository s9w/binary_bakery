#pragma once

#include <compare>


namespace bb
{

   struct div_t { int quot; int rem; };
   [[nodiscard]] constexpr auto div(const int x, const int y) -> div_t
   {
      return { x / y, x % y };
   }

   struct bit_count;

   struct byte_count {
      int m_value;

      template<std::integral integer_type>
      explicit constexpr byte_count(integer_type bytes)
         : m_value (static_cast<int>(bytes)) {}

      [[nodiscard]] constexpr auto get_bit_count() const -> bit_count;
      constexpr auto operator<=>(const byte_count&) const = default;
   };

   struct bit_count{
      int m_value;

      template<std::integral integer_type>
      explicit constexpr bit_count(integer_type bits)
         : m_value(static_cast<int>(bits)) {}

      [[nodiscard]] constexpr auto get_byte_count() const -> byte_count;
      constexpr auto operator<=>(const bit_count&) const = default;
   };


   template<typename... Ts>
   constexpr byte_count byte_sizeof = (0 + ... + sizeof(Ts));
   static_assert(byte_sizeof<double, int> == byte_count{ 12 });

   
} // namespace bb


constexpr auto bb::bit_count::get_byte_count() const -> byte_count
{
   const auto [full_bytes, rest] = bb::div(m_value, 8);
   int result_byte_count = full_bytes;
   if (rest > 0)
   {
      ++result_byte_count;
   }
   return byte_count{ result_byte_count };
}


constexpr auto bb::byte_count::get_bit_count() const -> bit_count
{
   return bit_count{ m_value * 8 };
}
