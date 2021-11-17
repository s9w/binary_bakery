#include <optional>

#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>


namespace {

   //auto my_error_function(
   //   std::string_view msg,
   //   const std::source_location& loc
   //) -> void
   //{
   //   throw std::exception{};
   //}

}

auto run_doctest() -> std::optional<int> {
   doctest::Context context;
   const int res = context.run();
   if (context.shouldExit())
      return res;
   return std::nullopt;
}


auto main() -> int
{
   //bb::error_callback = my_error_function;

   const std::optional<int> doctest_result = run_doctest();
   if (doctest_result.has_value())
      return doctest_result.value();
   return 0;
}

