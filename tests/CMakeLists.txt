project(bb_tests)

set(testRoot ${CMAKE_CURRENT_LIST_DIR})
configure_file(binary_bakery_testpaths.h.in include/binary_bakery_testpaths.h)

add_executable(
  ${PROJECT_NAME}
  color_tests.cpp
  config_tests.cpp
  decode_error_test.cpp
  decoding_tests_constexpr.cpp
  decoding_tests_lz4.cpp
  decoding_tests_uncompressed.cpp
  decoding_tests_zstd.cpp
  decoding_tools.cpp
  file_tools_tests.cpp
  image_tests.cpp
  payload_tests.cpp
  tests.cpp
  test_roundtrips.cpp
  tools_test.cpp
  universal_tests.cpp
  decoding_tools.h
  test_types.h
  ${CMAKE_CURRENT_BINARY_DIR}/include/binary_bakery_testpaths.h)

find_package(doctest CONFIG REQUIRED)
find_package(zstd CONFIG REQUIRED)
find_package(lz4 CONFIG REQUIRED)

target_link_libraries(
  ${PROJECT_NAME} PRIVATE doctest::doctest zstd::libzstd_static lz4::lz4
                          binary_bakery_lib binary_bakery_decoder)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC include
  PRIVATE src ${STB_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR}/include)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
set_target_properties(
  ${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY
                             "MultiThreaded$<$<CONFIG:Debug>:Debug>")

enable_testing()
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
