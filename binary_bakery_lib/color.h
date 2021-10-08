#pragma once

#include "tools.h"


namespace bb {

   template<int bpp>
   struct color {
      uint8_t m_components[bpp];

      auto operator<=>(const color<bpp>&) const = default;
      constexpr color(no_init) { }
      constexpr color(uint8_t r) requires(bpp == 1)
         : m_components{ r }
      {}
      constexpr color(uint8_t r, uint8_t a) requires(bpp == 2)
         : m_components{ r, a }
      {}
      constexpr color(uint8_t r, uint8_t g, uint8_t b) requires(bpp == 3)
         : m_components{ r, g, b }
      {}
      constexpr color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) requires(bpp == 4)
         : m_components{ r, g, b, a }
      {}
      [[nodiscard]] constexpr auto operator[](const int index) const -> uint8_t {
         return m_components[index];
      }
   };

   // Deduction guides
   color(uint8_t)->color<1>;
   color(uint8_t, uint8_t)->color<2>;
   color(uint8_t, uint8_t, uint8_t)->color<3>;
   color(uint8_t, uint8_t, uint8_t, uint8_t)->color<4>;

}
