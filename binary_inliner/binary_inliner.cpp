#include <iostream>
#include <format>

int main(int argc, char* argv[]) {
   for (int i = 0; i < argc; ++i)
      std::cout << std::format("{}: {}\n", i, argv[i]);

   if (argc != 2) {
      std::cout << "argument count != 2\n" << std::endl;
      return 0;
   }

   //stbi_set_flip_vertically_on_load(true);
   return 0;
}
