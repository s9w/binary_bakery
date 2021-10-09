#pragma once

#include "tools.h"


namespace bb {

   template<int bpp>
   struct color {
      uint8_t m_components[bpp];

      static constexpr auto black() -> color;

      constexpr color(no_init) { }
      constexpr color(uint8_t r)                                  requires(bpp == 1);
      constexpr color(uint8_t r, uint8_t a)                       requires(bpp == 2);
      constexpr color(uint8_t r, uint8_t g, uint8_t b)            requires(bpp == 3);
      constexpr color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) requires(bpp == 4);
      [[nodiscard]] constexpr auto operator[](const int index) const -> uint8_t;
      [[nodiscard]] constexpr auto operator[](const int index)       -> uint8_t&;
      [[nodiscard]] constexpr auto get_4byte_padded() const -> color<4>;
      auto operator<=>(const color<bpp>&) const = default;
   };

   // Deduction guides
   color(uint8_t)                            -> color<1>;
   color(uint8_t, uint8_t)                   -> color<2>;
   color(uint8_t, uint8_t, uint8_t)          -> color<3>;
   color(uint8_t, uint8_t, uint8_t, uint8_t) -> color<4>;

   template<int bpp>
   struct color_pair {
      color<bpp> m_color0;
      color<bpp> m_color1;
   };
}

template <int bpp>
constexpr auto bb::color<bpp>::black() -> color
{
   color black(no_init{});
   for (int i = 0; i < bpp; ++i)
      black[i] = 0ui8;
   return black;
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


template <int bpp>
constexpr auto bb::color<bpp>::get_4byte_padded() const -> color<4>
{
   color<4> result{ 0ui8, 0ui8, 0ui8, 0ui8 };
   for (int i = 0; i < bpp; ++i)
      result[i] = (*this)[i];
   return result;
}
