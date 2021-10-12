#include <chrono>
#include <format>
#include <iostream>

#include <binary_bakery_lib/config.h>
#include <binary_bakery_lib/file_tools.h>
#include <binary_bakery_lib/payload.h>

namespace bb {

   struct timer{
   private:
      std::chrono::time_point<std::chrono::high_resolution_clock> m_t0;
      std::string m_msg;

   public:
      explicit timer(const std::string& msg)
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

      timer(const timer&) = delete;
      timer& operator=(const timer&) = delete;
      timer(timer&&) = delete;
      timer& operator=(timer&&) = delete;
   };


   [[nodiscard]] auto get_config(const std::vector<payload>& payloads) -> config
   {
      // First: Try to get the config from the dirctory of the first file
      if(payloads.empty() == false)
      {
         const abs_directory_path payload_dir{ payloads[0].m_path.get_path().parent_path() };
         const std::optional<config> target_dir_cfg = get_cfg_from_dir(payload_dir);
         if (target_dir_cfg.has_value())
            return target_dir_cfg.value();
      }

      // If that fails, try to get it from the working directory
      const abs_directory_path working_dir{ fs::current_path() };
      const std::optional<config> working_dir_cfg = get_cfg_from_dir(working_dir);
      if (working_dir_cfg.has_value())
         return working_dir_cfg.value();

      // Lastly, use the default config
      config default_config;
      return default_config;
   }


   auto run(
      int argc,
      char* argv[]
   ) -> void
   {
      const abs_directory_path working_dir{ fs::path(argv[0]).parent_path() };

      std::vector<payload> payloads;
      payloads.reserve(argc-1);
      for (int i = 1; i < argc; ++i)
      {
         if (std::filesystem::exists(argv[i]) == false)
         {
            std::cout << std::format("Path doesn't exist: {} -> skipping.\n", argv[i]);
            continue;
         }
         if (std::filesystem::is_regular_file(argv[i]) == false)
         {
            std::cout << std::format("Path is not a file: {} -> skipping.\n", argv[i]);
            continue;
         }
         const byte_count file_size{ fs::file_size(argv[i]) };
         std::cout << std::format(
            "Packing file \"{}\". File size: {}, memory size: {}.",
            argv[i],
            get_human_readable_size(file_size),
            get_human_readable_size(get_file_memory_footprint(argv[i]))
         );

         payloads.emplace_back(get_payload(abs_file_path{ argv[i] }));
      }
      timer t(" Time to write");

      write_payloads_to_file(get_config(payloads), std::move(payloads), working_dir);
   }
}


auto main(
   int argc,
   char* argv[]
) -> int
{
   bb::run(argc, argv);

   return 0;
}
