# base/CMakeLists.txt
# -------------------
# The base library and its tests.
#
# Sources with std-only dependencies build on every supported platform. The
# pthread/POSIX-based components (mutex, condition_variable, thread, log,
# ...) are Linux-only for now. TODO: port them to Win32
# (thread_win.cc, SRWLOCK, CONDITION_VARIABLE, ...).
# -------------------

# CODEBASE_COMPONENT_BUILD (cmake/options.cmake): ON -> shared component,
# OFF -> static library.
if(CODEBASE_COMPONENT_BUILD)
  set(_base_library_type SHARED)
else()
  set(_base_library_type STATIC)
endif()

# std-only sources; portable across all supported platforms.
add_library(base ${_base_library_type}
  "command_line.cc"
  "concurrent/mutex_guard.cc"
  "concurrent/promise.cc"
  "concurrent/spin_lock.cc"
  "concurrent/task_queue_impl.cc"
  "concurrent/thread.cc"
  "file/path.cc"
  "memory/reference_counting.cc"
  "string_aux.cc"
  )

# The platform seam of Thread lives in per-platform backends.
if(WIN32)
  target_sources(base PRIVATE "concurrent/thread_win.cc")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_sources(base PRIVATE "concurrent/thread_linux.cc")
endif()

# pthread/POSIX-only sources.
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_sources(base PRIVATE
    "concurrent/blocking_queue.cc"
    "concurrent/condition_variable.cc"
    "concurrent/count_down_latch.cc"
    "concurrent/mutex.cc"
    "concurrent/restrict_mutex.cc"
    "concurrent/thread_group.cc"
    "concurrent/task_executor_impl.cc"
    "log.cc"
    )
endif()

# Repo-root include path is part of base's usage requirements ("base/..."
# includes appear in its public headers), so link the config PUBLICly.
target_link_libraries(base PUBLIC codebase::config)

# Export macro wiring (see base/base_exports.h): a component build exports
# symbols from the shared library (BASE_IMPLEMENTATION selects dllexport while
# compiling base itself, consumers get dllimport); a static build disables
# dllimport for both base and its consumers, hence PUBLIC.
if(CODEBASE_COMPONENT_BUILD)
  target_compile_definitions(base PRIVATE BASE_IMPLEMENTATION)
else()
  target_compile_definitions(base PUBLIC BASE_STATIC_DEFINE)
  # Static archives are not linked with -fPIC objects by default; enable it so
  # base.a can also be linked into shared libraries later.
  set_target_properties(base PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Portable tests.
add_executable(spin_lock "concurrent/spin_lock_tests.cc")
target_link_libraries(spin_lock PRIVATE base codebase::config third_party::gtest)

add_executable(reference_counting_tests "memory/reference_counting_tests.cc")
target_link_libraries(reference_counting_tests PRIVATE base codebase::config third_party::gtest)

add_executable(path_tests "file/path_tests.cc")
target_link_libraries(path_tests PRIVATE base codebase::config third_party::gtest)

add_executable(thread_tests "concurrent/thread_tests.cc")
target_link_libraries(thread_tests PRIVATE base codebase::config third_party::gtest)

add_executable(move_only_function_tests "functional/move_only_function_tests.cc")
target_link_libraries(move_only_function_tests PRIVATE base codebase::config third_party::gtest)

add_executable(closure_tests "functional/closure_tests.cc")
target_link_libraries(closure_tests PRIVATE base codebase::config third_party::gtest)

# ASan only on Linux for tests that link base: on Windows enable_asan forces
# the release CRT (ASan does not support /MDd), which clashes with the
# debug-CRT base.lib in Debug configs (lld-link /failifmismatch).
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  enable_asan(path_tests)
  enable_asan(thread_tests)
endif()

# Linux-only tests (they exercise the pthread/POSIX-based components).
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  add_executable(task_executor_tests "concurrent/task_executor_tests.cc")
  target_link_libraries(task_executor_tests PRIVATE base codebase::config third_party::gtest)

  add_executable(log_tests "log_tests.cc")
  target_link_libraries(log_tests PRIVATE base codebase::config third_party::gtest)
  enable_asan(log_tests)

  add_executable(count_down_latch_tests "concurrent/count_down_latch_tests.cc")
  target_link_libraries(count_down_latch_tests PRIVATE base codebase::config third_party::gtest)
endif()
