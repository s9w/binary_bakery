#include <binary_bakery_lib/config.h>

#include <doctest/doctest.h>

using namespace bb;


TEST_CASE("config files")
{
   const auto cfg = read_config_from_toml(abs_file_path{ "test_configs/c0.toml" });
   CHECK_EQ(cfg.output_filename, "123.h");
   CHECK_EQ(cfg.smart_mode, false);
   CHECK_EQ(cfg.compression, compression_mode::zstd);
}
