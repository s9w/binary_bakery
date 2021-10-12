#include <binary_bakery_lib/config.h>

#include <doctest/doctest.h>

using namespace bb;


TEST_CASE("config files")
{
   const auto cfg = get_cfg_from_dir(abs_directory_path{ "test_configs" }).value();
   CHECK_EQ(cfg.output_filename, "123.h");
   CHECK_EQ(cfg.smart_mode, false);
   CHECK_EQ(cfg.compression, compression_mode::zstd);
}
