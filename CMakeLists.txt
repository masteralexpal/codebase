cmake_minimum_required(VERSION 3.25)

project("codebase" C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Builder-togglable project options, compiler policy (per-platform supported
# toolchains) and enable_xxx(<target>) feature modules with platform
# translation.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(options)
include(compiler)
include(warnings)
include(sanitizer)
include(lto)

codebase_check_compiler()

# Monorepo configuration, propagated to first-party targets explicitly via
# codebase::config (repo-root include path + warning flags + platform libs).
# Third-party targets are never affected.
add_library(codebase_config INTERFACE)
add_library(codebase::config ALIAS codebase_config)
target_include_directories(codebase_config INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
enable_warnings(codebase_config)

# Builder-togglable features (cmake -DCODEBASE_ENABLE_LTO=ON).
if(CODEBASE_ENABLE_LTO)
  enable_lto(codebase_config)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_link_libraries(codebase_config INTERFACE "pthread" "rt")
endif()

# Third party libraries (vcpkg), exposed as third_party:: aliases.
add_subdirectory("third_party")

# The base library and its tests (Linux-only for now, see base/CMakeLists.txt).
add_subdirectory("base")

add_executable(hello_world hello_world.cc)
target_link_libraries(hello_world PRIVATE codebase::config)

# Experiments
add_subdirectory("experiment")
