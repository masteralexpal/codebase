# cmake/common.cmake
# ------------------
# Helpers shared by the enable_xxx feature modules. Not meant to be included
# by CMakeLists.txt directly; feature modules pull it in themselves.
# ------------------
include_guard(GLOBAL)

# codebase_option_scope(<target> <out-var>)
# Resolves the propagation scope for a target: PRIVATE for buildable targets,
# INTERFACE for interface-only libraries. The latter lets the monorepo config
# target be passed to enable_xxx functions too, e.g.
# enable_warnings(codebase::config), propagating the options to every
# consumer.
function(codebase_option_scope target out_var)
  get_target_property(_type ${target} TYPE)
  if(_type STREQUAL "INTERFACE_LIBRARY")
    set(${out_var} INTERFACE PARENT_SCOPE)
  else()
    set(${out_var} PRIVATE PARENT_SCOPE)
  endif()
endfunction()
