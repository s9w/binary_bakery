#include <doctest/doctest.h>

#include <binary_bakery_decoder.h>
#include "test_images/test_payload_rgb.h"

#include "test_types.h"

using namespace bb;


namespace tests {
   
   TEST_CASE("get_element()")
   {
      constexpr const uint64_t* ptr = bb::get_payload("test_image_rgb.png");
      static_assert(bb::get_element<nc_test_rgb>(ptr, 0) == nc_test_rgb{ 255, 0, 0 });
      static_assert(bb::get_element<nc_test_rgb>(ptr, 1) == nc_test_rgb{ 0, 255, 0 });
      static_assert(bb::get_element<nc_test_rgb>(ptr, 2) == nc_test_rgb{ 0, 0, 255 });
      static_assert(bb::get_element<nc_test_rgb>(ptr, 3) == nc_test_rgb{ 0, 0, 0 });
   }

}
