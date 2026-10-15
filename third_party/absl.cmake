find_package(absl CONFIG REQUIRED)
add_library(third_party_absl INTERFACE)
target_link_libraries(third_party_absl INTERFACE absl::any_invocable)
add_library(third_party::absl ALIAS third_party_absl)
