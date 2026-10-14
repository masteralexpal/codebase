# cmake/warnings.cmake
# --------------------
# Warning level helpers with platform translation.
# --------------------
include_guard(GLOBAL)
include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

# enable_warnings(<target>)
# /W4 for MSVC-like compilers (cl, clang-cl), -Wall -Wextra otherwise, plus
# -Werror in non-Debug builds for GNU-like compilers.
function(enable_warnings target)
  codebase_option_scope(${target} _scope)
  if(MSVC)
    target_compile_options(${target} ${_scope} "/W4")
  else()
    target_compile_options(${target} ${_scope}
      "-Wall" "-Wextra" "$<$<NOT:$<CONFIG:Debug>>:-Werror>")
  endif()
endfunction()
