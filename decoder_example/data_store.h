#pragma once

#include <cstdint>
#include <compare>
#include <vector>

#define BAKERY_PROVIDE_VECTOR
#define BAKERY_PROVIDE_STD_ARRAY
#include "../binary_bakery_decoder.h"

namespace example {
   struct user_type {
      uint8_t r{}, g{}, b{};
      auto operator<=>(const user_type&) const = default;
   };

   auto decode_to_vector(const char* name) -> std::vector<user_type>;
}