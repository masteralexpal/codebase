find_package(benchmark CONFIG REQUIRED)
add_library(third_party_benchmark INTERFACE)
target_link_libraries(third_party_benchmark INTERFACE benchmark::benchmark benchmark::benchmark_main)
add_library(third_party::benchmark ALIAS third_party_benchmark)
