# base/CMakeLists.txt
# -------------------
# The base library and its tests.
#
# NOTE: base is currently pthread/Linux-only (mutex.h, thread.cc, log.cc etc.
# include <pthread.h> and <sys/...> unconditionally), so nothing is built on
# other platforms. TODO: port to Win32 (platform_thread_win, SRWLOCK,
# CONDITION_VARIABLE, ...).
# -------------------

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
  return()
endif()

# CODEBASE_COMPONENT_BUILD (cmake/options.cmake): ON -> shared component,
# OFF -> static library.
if(CODEBASE_COMPONENT_BUILD)
  set(_base_library_type SHARED)
else()
  set(_base_library_type STATIC)
endif()

add_library(base ${_base_library_type}
  "concurrent/blocking_queue.cc"
  "concurrent/condition_variable.cc"
  "concurrent/count_down_latch.cc"
  "concurrent/mutex_guard.cc"
  "concurrent/mutex.cc"
  "concurrent/promise.cc"
  "concurrent/platform_thread_linux.cc"
  "concurrent/restrict_mutex.cc"
  "concurrent/spin_lock.cc"
  "concurrent/thread.cc"
  "concurrent/thread_group.cc"
  "concurrent/task_executor_impl.cc"
  "concurrent/task_queue_impl.cc"
  "command_line.cc"
  "memory/atomic_reference_counting.cc"
  "memory/reference_counting.cc"
  "file/path.cc"
  "log.cc"
  "platform/platform.cc"
  "string_aux.cc"
  )
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

# concurrent tests
add_executable(thread_tests "concurrent/thread_tests.cc")
target_link_libraries(thread_tests PRIVATE base codebase::config third_party::gtest)
enable_asan(thread_tests)

add_executable(platform_thread_linux_tests "concurrent/platform_thread_linux_tests.cc")
target_link_libraries(platform_thread_linux_tests PRIVATE base codebase::config third_party::gtest)
enable_asan(platform_thread_linux_tests)

add_executable(task_executor_tests "concurrent/task_executor_tests.cc")
target_link_libraries(task_executor_tests PRIVATE base codebase::config third_party::gtest)

add_executable(log_tests "log_tests.cc")
target_link_libraries(log_tests PRIVATE base codebase::config third_party::gtest)
enable_asan(log_tests)

add_executable(count_down_latch_tests "concurrent/count_down_latch_tests.cc")
target_link_libraries(count_down_latch_tests PRIVATE base codebase::config third_party::gtest)

add_executable(spin_lock "concurrent/spin_lock_tests.cc")
target_link_libraries(spin_lock PRIVATE base codebase::config third_party::gtest)

add_executable(reference_counting_tests "memory/reference_counting_tests.cc")
target_link_libraries(reference_counting_tests PRIVATE base codebase::config third_party::gtest)

add_executable(utility_tests "utility_tests.cc")
target_link_libraries(utility_tests PRIVATE base codebase::config third_party::gtest)

add_executable(shared_ptr_tests "memory/shared_ptr_tests.cc")
target_link_libraries(shared_ptr_tests PRIVATE base codebase::config third_party::gtest)
enable_asan(shared_ptr_tests)

add_executable(path_tests "file/path_tests.cc")
target_link_libraries(path_tests PRIVATE base codebase::config third_party::gtest)
enable_asan(path_tests)

# platform tests
add_executable(platform_tests "platform/platform_tests.cc")
target_link_libraries(platform_tests PRIVATE base codebase::config third_party::gtest)
enable_asan(platform_tests)
