#include <doctest/doctest.h>

#include <binary_bakery_lib/file_tools.h>

using namespace bb;

TEST_CASE("abs_directory_path")
{
   SUBCASE("must be a directory")
   {
      CHECK_THROWS_AS(abs_directory_path{ "tests.cpp" }, std::exception);
      CHECK_NOTHROW(abs_directory_path{ "C:/" });
      CHECK_NOTHROW(abs_directory_path{ "C:/Dropbox" });
      CHECK_NOTHROW(abs_directory_path{ "." });
   }
   SUBCASE("must exist")
   {
      CHECK_THROWS_AS(abs_directory_path{ "C:/doesnt_exist" }, std::exception);
   }
}


TEST_CASE("abs_file_path")
{
   SUBCASE("must be a file")
   {
      CHECK_THROWS_AS(abs_file_path{ "." }, std::exception);
      CHECK_NOTHROW(abs_file_path{ "tests.cpp" });
   }
   SUBCASE("must exist")
   {
      CHECK_THROWS_AS(abs_file_path{ "doesnt_exist.cpp" }, std::exception);
   }
}
