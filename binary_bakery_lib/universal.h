#pragma once

#include <compare>
#include <variant>

namespace bb::detail
{
   template<typename T>
   struct is_variant : std::false_type {};
   template<typename ...Args>
   struct is_variant<std::variant<Args...>> : std::true_type {};

   template<class alternative_type, typename variant_type>
   struct is_alternative
   {
      static_assert(sizeof(alternative_type) < 0, "Can't use is_alternative with non-variant type");
   };
   template<typename alternative_type, typename... variant_alternatives>
   struct is_alternative<alternative_type, std::variant<variant_alternatives...>>
      : std::disjunction<std::is_same<alternative_type, variant_alternatives>...>
   { };
}

namespace bb
{

   struct no_init {};

   template<typename T>
   concept numerical = (std::integral<T> && !std::is_same_v<T, bool>) || std::floating_point<T>;

   template<typename T>
   inline constexpr bool is_variant_v = detail::is_variant<T>::value;

   // is_alternative_v<A, B> returns if A is contained in the variant type B
   template<class alternative_type, class variant_type>
   constexpr bool is_alternative_v = detail::is_alternative<alternative_type, variant_type>::value;

   template<typename T, typename variant_type>
   concept alternative_of = is_alternative_v<T, variant_type>;

   template<bb::numerical T>
   [[nodiscard]] constexpr auto abs(const T value) noexcept -> T;

   template<bb::numerical T>
   [[nodiscard]] constexpr auto equal(const T a, const T b) noexcept -> bool;

   struct div_t { int quot; int rem; };
   [[nodiscard]] constexpr auto div(const int x, const int y) -> div_t;

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


template<bb::numerical T>
constexpr auto bb::abs(const T value) noexcept -> T
{
   if constexpr (std::is_unsigned_v<T>)
      return value;
   else
      return value < T{} ? -value : value;
}


template<bb::numerical T>
constexpr auto bb::equal(
   const T a,
   const T b
) noexcept -> bool
{
   constexpr double tol = 0.001;
   if constexpr (std::is_integral_v<T>)
      return a == b;
   else
      return bb::abs(a - b) <= tol;
}


constexpr auto bb::div(const int x, const int y) -> div_t
{
   return { x / y, x % y };
}
