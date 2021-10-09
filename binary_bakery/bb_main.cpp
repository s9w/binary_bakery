#include <format>
#include <iostream>

#include <binary_bakery_lib/config.h>

namespace bb {

   auto run() -> void
   {
      const fs::path path = R"(C:/Dropbox/code/binary_inliner/x64/Debug/binary_bakery.toml)";
      const auto config = read_config_from_toml(path);
   }

}


auto main(int argc, char* argv[]) -> int
{
   bb::run();

   // TODO Check if params are valid files
   for (int i = 0; i < argc; ++i)
   {
      std::cout << std::format("{}: {}\n", i, argv[i]);
   }

   char in;
   std::cin >> in;

   return 0;
}
