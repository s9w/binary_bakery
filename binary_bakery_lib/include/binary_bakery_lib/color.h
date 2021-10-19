#pragma once

#include <binary_bakery_lib/universal.h>
#include <cstdint>

namespace bb {

   template<int bpp>
   struct color {
      uint8_t m_components[bpp];

      constexpr explicit color(no_init) { }
      constexpr explicit color(uint8_t r)                                  requires(bpp == 1);
      constexpr explicit color(uint8_t r, uint8_t a)                       requires(bpp == 2);
      constexpr explicit color(uint8_t r, uint8_t g, uint8_t b)            requires(bpp == 3);
      constexpr explicit color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) requires(bpp == 4);
      [[nodiscard]] constexpr auto operator[](const int index) const -> uint8_t;
      [[nodiscard]] constexpr auto operator[](const int index)       -> uint8_t&;
      auto operator<=>(const color<bpp>&) const = default;
   };

   // Deduction guides
   color(uint8_t)                            -> color<1>;
   color(uint8_t, uint8_t)                   -> color<2>;
   color(uint8_t, uint8_t, uint8_t)          -> color<3>;
   color(uint8_t, uint8_t, uint8_t, uint8_t) -> color<4>;
}


template <int bpp>
constexpr bb::color<bpp>::color(uint8_t r) requires (bpp == 1)
   : m_components{ r }
{
   
}


template <int bpp>
constexpr bb::color<bpp>::color(uint8_t r, uint8_t a) requires (bpp == 2)
   : m_components{ r, a }
{
   
}


template <int bpp>
constexpr bb::color<bpp>::color(uint8_t r, uint8_t g, uint8_t b) requires (bpp == 3)
   : m_components{ r, g, b }
{
   
}


template <int bpp>
constexpr bb::color<bpp>::color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) requires (bpp == 4)
   : m_components{ r, g, b, a }
{
   
}


template <int bpp>
constexpr auto bb::color<bpp>::operator[](const int index) const -> uint8_t
{
   return m_components[index];
}


template <int bpp>
constexpr auto bb::color<bpp>::operator[](const int index) -> uint8_t& 
{
   return m_components[index];
}
