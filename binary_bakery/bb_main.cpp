#include <format>
#include <iostream>
#include <chrono>

#include <binary_bakery_lib/config.h>
#include <binary_bakery_lib/payload.h>

namespace bb {

   struct timer{
      std::chrono::time_point<std::chrono::high_resolution_clock> m_t0;
      timer()
         : m_t0(std::chrono::high_resolution_clock::now())
      {}
      ~timer()
      {
         const auto t1 = std::chrono::high_resolution_clock::now();
         const auto us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - m_t0).count();
         const double ms = us / 1000.0;
         printf("time: %fms\n", ms);
      }

   };

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
            timer t;
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
