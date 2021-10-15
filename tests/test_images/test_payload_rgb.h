#include <cstdint>
#include <cstring>

namespace bb{
static constexpr uint64_t bb_test_image_rgb_png[]{
   0x0002000303000001, 0x0000001200000012, 0x000000ff000000ff, 0x00ffffff000000ff,
   0x0000000000000000
};

[[nodiscard]] constexpr auto is_equal_c_string(
   char const* first,
   char const* second
) -> bool
{
   if(std::is_constant_evaluated() == false)
      return strcmp(first, second) == 0;

   return *first == *second &&
      (*first == '\0' || is_equal_c_string(&first[1], &second[1]));
}

[[nodiscard]] static constexpr auto get_payload(
   [[maybe_unused]] const char* name
) -> const uint64_t*
{
   if(is_equal_c_string(name, "test_image_rgb.png"))
      return &bb_test_image_rgb_png[0];
   else
      return nullptr;
}

} // namespace bb
