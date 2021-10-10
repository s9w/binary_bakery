#include <vector>

#define BAKERY_PROVIDE_VECTOR
#define BAKERY_PROVIDE_STD_ARRAY
#include "../decoder.h"

#include "../binary_bakery/bb_collection.h"

struct user_type {
   uint8_t r{}, g{}, b{};
   auto operator<=>(const user_type&) const = default;
};

int main()
{
   constexpr bb::header decoded_header = bb::get_header(stress_test_png);

   printf("data size: %iKB\n", decoded_header.bit_count/8/1024);

   //const auto decoded_array = bb::decode_to_array<user_type, decoded_header>(stress_test_png);
   //constexpr auto decoded_array = bb::decode_to_array<user_type, decoded_header>(stress_test_png);
   //printf("%i\n", decoded_array[0].r);

   const std::vector<user_type> vec = bb::decode_to_vector<user_type>(stress_test_png);
   //printf("%i bytes\n", vec.size());
   

   return 0;
}
