#include <format>
#include <iostream>
#include <chrono>

#include <binary_bakery_lib/config.h>
#include <binary_bakery_lib/payload.h>

namespace bb {

   struct timer{
      std::chrono::time_point<std::chrono::high_resolution_clock> m_t0;
      std::string m_msg;
      timer(const std::string& msg)
         : m_t0(std::chrono::high_resolution_clock::now())
         , m_msg(msg)
      {}
      ~timer()
      {
         const auto t1 = std::chrono::high_resolution_clock::now();
         const auto us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - m_t0).count();
         const double ms = us / 1000.0;
         std::cout << std::format("{}: {}ms\n", m_msg, ms);
      }
   };



   [[nodiscard]] auto get_best_config_dir(
      const std::vector<payload>& payloads
   ) -> fs::path
   {
      if (payloads.empty())
      {
         return fs::current_path();
      }
      else
      {
         return payloads[0].m_path.parent_path();
      }
   }


   auto run(
      int argc,
      char* argv[]
   ) -> void
   {
      const fs::path working_dir = fs::path(argv[0]).parent_path();

      std::vector<payload> payloads;
      for (int i = 0; i < argc; ++i)
      {
         if(i == 0)
            continue;
         if (std::filesystem::exists(argv[i]) == false)
         {
            std::cout << std::format("File doesn't exist: {} -> skipping.\n", argv[i]);
            continue;
         }
         const byte_count file_size{ fs::file_size(argv[i]) };
         std::cout << std::format(
            "Packing file {}. File size: {}, memory footprint: {}.\n",
            argv[i],
            get_human_readable_size(file_size),
            get_human_readable_size(get_file_memory_footprint(argv[i]))
         );

         payloads.emplace_back(get_payload(argv[i]));
      }
      timer t("Time to write");

      const fs::path config_path = get_best_config_dir(payloads) / fs::path("binary_bakery.toml");
      const auto config = read_config_from_toml(config_path);
      write_payloads_to_file(config, std::move(payloads), working_dir);
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
