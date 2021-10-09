#include <vector>

#define BAKERY_PROVIDE_VECTOR
#include "../decoder.h"

#include "../binary_bakery/bb_collection.h"

struct user_type {
   uint8_t r{}, g{}, b{};
   auto operator<=>(const user_type&) const = default;
};

int main()
{
   constexpr bb::header decoded_header = bb::get_header(stress_test_png);
   const std::vector<user_type> vec = bb::decode_to_vector<user_type>(stress_test_png);
   const int ss = vec.size();
   printf("%i\n", ss);

   return 0;
}
