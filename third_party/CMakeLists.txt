# third_party/CMakeLists.txt
# ---------------------------
# Third party libraries (provided by vcpkg). Each dependency gets a stable
# third_party:: alias so consumers never name vendor targets directly; swapping
# the provider only means editing the single .cmake file below.
# ---------------------------

include(absl.cmake)
include(benchmark.cmake)
include(gtest.cmake)
