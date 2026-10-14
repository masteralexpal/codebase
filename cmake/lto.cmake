# cmake/lto.cmake
# ---------------
# Link-time optimization helpers.
# ---------------
include_guard(GLOBAL)
include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

# enable_lto(<target>)
# Enables link-time optimization with platform translation:
#   gcc / clang / clang-cl : -flto at compile time. On Unix the link step goes
#                            through the compiler driver, which needs -flto to
#                            engage the linker plugin; on Windows CMake drives
#                            links through lld-link directly, which runs LTO
#                            automatically on LLVM bitcode inputs.
# Works on interface targets too, so the monorepo config target can be passed
# to enable LTO repo-wide.
#
# PITFALL: do not "simplify" this into CMake's official mechanism,
#   set_property(TARGET <t> PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
# That property does NOT propagate through interface libraries (and there is
# no INTERFACE_INTERPROCEDURAL_OPTIMIZATION counterpart). Worse, set_property
# accepts arbitrary property names without validation, so setting it on an
# interface target fails silently: no error, no warning, and no LTO in the
# generated build. Explicit compile/link options are used here precisely
# because they DO propagate through interface libraries.
function(enable_lto target)
  codebase_option_scope(${target} _scope)
  target_compile_options(${target} ${_scope} "-flto")
  if(NOT MSVC)
    target_link_options(${target} ${_scope} "-flto")
  endif()
endfunction()
