project(binary_bakery)
add_executable(${PROJECT_NAME} bb_main.cpp)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt binary_bakery_lib)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

set_target_properties(
  ${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY
                             "MultiThreaded$<$<CONFIG:Debug>:Debug>")

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION .)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/../binary_bakery_decoder/include/
        DESTINATION .)
install(FILES "${PROJECT_SOURCE_DIR}/../LICENSE.txt" "${PROJECT_SOURCE_DIR}/../binary_bakery.toml" DESTINATION .)
