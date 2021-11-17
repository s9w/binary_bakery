#pragma once
#include <cstdint>
#include <cstring>
#include <string_view>

namespace bb{
static constexpr uint64_t bb_test_image_rgb_png[]{
   0x0002000303000001, 0x0000001200000012, 0x000000ff000000ff, 0x00ffffff000000ff,
   0x0000000000000000
};

[[nodiscard]] static constexpr auto get_payload(
   [[maybe_unused]] std::string_view name
) -> const uint64_t*
{
   if(name == "test_image_rgb.png")
      return &bb_test_image_rgb_png[0];
   else
      return nullptr;
}

} // namespace bb
