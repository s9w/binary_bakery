cmake_minimum_required(VERSION 3.17)

if(${BUILD_VCPKG})
  # Used by CI/CD, can be used to jump start the project
  include(.github/bb_buildVCPKG.cmake)
  bb_buildvcpkg()
  set(CMAKE_TOOLCHAIN_FILE "${vcpkgPath}/scripts/buildsystems/vcpkg.cmake")
  set(VCPKG_TARGET_TRIPLET
      ${vcpkgTriplet}
      CACHE STRING "VCPKG Target Triplet to use")
  set(CMAKE_INSTALL_PREFIX
      "${CMAKE_BINARY_DIR}/artifacts"
      CACHE PATH "default CI install path" FORCE)
endif(${BUILD_VCPKG})

project(BinaryBakery)

add_subdirectory(binary_bakery_decoder)
add_subdirectory(binary_bakery_lib)
add_subdirectory(binary_bakery)
include(CTest)
add_subdirectory(tests)
