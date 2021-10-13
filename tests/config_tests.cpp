#include <binary_bakery_lib/config.h>

#include <doctest/doctest.h>

using namespace bb;


TEST_CASE("get_cfg_from_dir()")
{
   const auto cfg = get_cfg_from_dir(abs_directory_path{ "test_configs" }).value();
   CHECK_EQ(cfg.output_filename, "123.h");
   CHECK_EQ(cfg.smart_mode, false);
   CHECK_EQ(cfg.compression, compression_mode::zstd);
}

TEST_CASE("get_cfg_from_file()")
{
   const auto cfg = get_cfg_from_file(abs_file_path{ "test_configs/c0.toml" }).value();
   CHECK_EQ(cfg.output_filename, "123.h");
   CHECK_EQ(cfg.smart_mode, false);
   CHECK_EQ(cfg.compression, compression_mode::zstd);
   CHECK_EQ(cfg.image_loading_direction, image_vertical_direction::top_to_bottom);
}
