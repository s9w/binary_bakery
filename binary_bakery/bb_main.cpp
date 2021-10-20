#include <chrono>

#include <binary_bakery_lib/config.h>
#include <binary_bakery_lib/file_tools.h>
#include <binary_bakery_lib/payload.h>

#if defined(_WIN32) || defined(WIN32)
#include <conio.h>
#endif

#include <fmt/format.h>

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
         fmt::print("{}: {}ms\n", m_msg, ms);
      }

      timer(const timer&) = delete;
      timer& operator=(const timer&) = delete;
      timer(timer&&) = delete;
      timer& operator=(timer&&) = delete;
   };


   // move this to config header?
   [[nodiscard]] auto get_config(
      const std::vector<abs_file_path>& packing_files
   ) -> config
   {
      // First: Try to get the config from the directory of the first file
      if(packing_files.empty() == false)
      {
         const abs_directory_path payload_dir{ packing_files[0].get_path().parent_path() };
         const std::optional<config> target_dir_cfg = get_cfg_from_dir(payload_dir);
         if (target_dir_cfg.has_value())
         {
            fmt::print("Using config from \"{}\".\n", payload_dir.get_path().string());
            return target_dir_cfg.value();
         }
      }

      // If that fails, try to get it from the working directory
      const abs_directory_path working_dir{ fs::current_path() };
      const std::optional<config> working_dir_cfg = get_cfg_from_dir(working_dir);
      if (working_dir_cfg.has_value())
      {
         fmt::print("Using config from \"{}\".\n", working_dir.get_path().string());
         return working_dir_cfg.value();
      }


      // Lastly, use the default config
      config default_config;
      fmt::print("Using default config.\n");
      return default_config;
   }


   // This is a surprisingly difficult problem with no good universal solution.
   // For the moment, only do this on windows.
   auto wait_for_keypress() -> void
   {
#if defined(_WIN32) || defined(WIN32)
      fmt::print("\nPress any key to continue\n");
      [[maybe_unused]] const auto input_val = _getch();
#endif
   }

   // From the inputs, filter out all non-files and non-existing files. Also try to find a config
   // among the parameters
   struct input_files {
      std::vector<abs_file_path> m_packing_files;
      std::optional<config> m_config;
   };


   [[nodiscard]] auto get_input_files(
      int argc,
      char* argv[]
   ) -> input_files
   {
      input_files result;
      result.m_packing_files.reserve(argc - 1);
      for (int i = 1; i < argc; ++i)
      {
         // Path must exist and must be a file
         if (std::filesystem::exists(argv[i]) == false)
         {
            fmt::print("Path doesn't exist: {} -> skipping.\n", argv[i]);
            continue;
         }
         if (std::filesystem::is_regular_file(argv[i]) == false)
         {
            fmt::print("Path is not a file: {} -> skipping.\n", argv[i]);
            continue;
         }

         const abs_file_path file{ argv[i] };

         if (file.get_path().extension() == ".toml")
         {
            const std::optional<config> explicit_config = get_cfg_from_file(file);
            if (explicit_config.has_value())
            {
               if (result.m_config.has_value())
               {
                  fmt::print("There was already a config among the input files. Ignoring this one\n");
                  continue;
               }
               fmt::print("Using explicit config file \"{}\".\n", file.get_path().string());
               result.m_config.emplace(explicit_config.value());
               continue;
            }
         }
         result.m_packing_files.emplace_back(file);
      }
      return result;
   }


   auto run(
      int argc,
      char* argv[]
   ) -> void
   {
      const input_files inputs = get_input_files(argc, argv);
      const config cfg = [&]() {
         if (inputs.m_config.has_value())
            return inputs.m_config.value();
         else
            return get_config(inputs.m_packing_files);
      }();

      std::vector<payload> payloads;
      payloads.reserve(inputs.m_packing_files.size());
      for (const abs_file_path& file : inputs.m_packing_files)
      {
         payloads.emplace_back(get_payload(file, cfg));
      }

      {
         timer t("Time to write");
         const abs_directory_path working_dir{ fs::current_path() };
         write_payloads_to_file(cfg, std::move(payloads), working_dir);
      }

      if (cfg.prompt_for_key)
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
