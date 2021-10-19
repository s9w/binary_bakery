project(binary_bakery_lib)
add_library(
  ${PROJECT_NAME}
  include/binary_bakery_lib/color.h
  src/compression.cpp
  include/binary_bakery_lib/compression.h
  src/config.cpp
  include/binary_bakery_lib/config.h
  src/content_meta.cpp
  include/binary_bakery_lib/content_meta.h
  src/file_tools.cpp
  include/binary_bakery_lib/file_tools.h
  src/image.cpp
  include/binary_bakery_lib/image.h
  src/implementations.cpp
  src/payload.cpp
  include/binary_bakery_lib/payload.h
  src/tools.cpp
  include/binary_bakery_lib/tools.h
  include/binary_bakery_lib/universal.h)

find_package(zstd CONFIG REQUIRED)
find_package(lz4 CONFIG REQUIRED)
find_package(tomlplusplus CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_path(STB_INCLUDE_DIRS "stb.h")
target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE zstd::libzstd_static lz4::lz4 tomlplusplus::tomlplusplus fmt::fmt
          binary_bakery_decoder)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC include
  PRIVATE src ${STB_INCLUDE_DIRS})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
set_target_properties(
  ${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY
                             "MultiThreaded$<$<CONFIG:Debug>:Debug>")
