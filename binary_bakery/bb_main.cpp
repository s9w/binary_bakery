#include <format>
#include <iostream>

#include <binary_bakery_lib/config.h>
#include <binary_bakery_lib/payload.h>

namespace bb {

   auto run(
      int argc,
      char* argv[]
   ) -> void
   {
      const fs::path path = R"(C:/Dropbox/code/binary_inliner/x64/Debug/binary_bakery.toml)";
      const auto config = read_config_from_toml(path);

      // TODO Check if params are valid files
      for (int i = 0; i < argc; ++i)
      {
         std::cout << std::format("{}: {}\n", i, argv[i]);
         if (i >= 1)
         {
            auto payload = get_payload(argv[i]);
            write_payload_to_file(config, std::move(payload));
         }
      }
   }

}


auto main(
   int argc,
   char* argv[]
) -> int
{
   bb::run(argc, argv);

   //char in;
   //std::cin >> in;

   return 0;
}
