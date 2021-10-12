#include <chrono>
#include <format>
#include <iostream>

#include <binary_bakery_lib/config.h>
#include <binary_bakery_lib/file_tools.h>
#include <binary_bakery_lib/payload.h>

#if defined(_WIN32) || defined(WIN32)
#include <conio.h>
#endif

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
         {
            std::cout << std::format("Using config from \"{}\".\n", payload_dir.get_path().string());
            return target_dir_cfg.value();
         }
      }

      // If that fails, try to get it from the working directory
      const abs_directory_path working_dir{ fs::current_path() };
      const std::optional<config> working_dir_cfg = get_cfg_from_dir(working_dir);
      if (working_dir_cfg.has_value())
      {
         std::cout << std::format("Using config from \"{}\".\n", working_dir.get_path().string());
         return working_dir_cfg.value();
      }


      // Lastly, use the default config
      config default_config;
      std::cout << std::format("Using default config.\n");
      return default_config;
   }


   // This is a surprisingly difficult problem with no good universal solution.
   // For the moment, only do this on windows.
   auto wait_for_keypress() -> void
   {
#if defined(_WIN32) || defined(WIN32)
      std::cout << std::format("\nPress any key to continue\n");
      [[maybe_unused]] const auto input_val = _getch();
#endif
   }


   auto run(
      int argc,
      char* argv[]
   ) -> void
   {
      const abs_directory_path working_dir{ fs::path(argv[0]).parent_path() };

      std::vector<payload> payloads;
      payloads.reserve(argc-1);
      std::optional<config> cfg;
      for (int i = 1; i < argc; ++i)
      {
         // Path must exist and must be a file
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

         // Try to see if it's an explicit config file
         const abs_file_path file{ argv[i] };
         if (file.get_path().extension() == ".toml")
         {
            const std::optional<config> explicit_config = get_cfg_from_file(file);
            if (explicit_config.has_value())
            {
               if (cfg.has_value())
               {
                  std::cout << "There's more than one explicit config file among the parameters. I hope you know what you're doing.\n";
               }
               std::cout << std::format("Using explicit config file \"{}\".\n", file.get_path().string());
               cfg.emplace(explicit_config.value());
            }
         }
         else
         {
            payloads.emplace_back(get_payload(abs_file_path{ argv[i] }));
         }
      }
      if (cfg.has_value() == false)
      {
         cfg = get_config(payloads);
      }

      {
         timer t("Time to write");
         write_payloads_to_file(cfg.value(), std::move(payloads), working_dir);
      }

      if (cfg.value().prompt_for_key)
         wait_for_keypress();
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
