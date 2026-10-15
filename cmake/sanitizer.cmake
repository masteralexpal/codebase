# cmake/sanitizer.cmake
# ---------------------
# Sanitizer helpers with platform translation.
# ---------------------
include_guard(GLOBAL)
include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

# codebase_clang_rt_windows_dir(<out-var>)
# Locates Clang's Windows runtime library directory
# (<llvm>/lib/clang/<version>/lib/windows) relative to the compiler.
# Sets an empty string when not found.
function(codebase_clang_rt_windows_dir out_var)
  get_filename_component(_bin_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
  file(GLOB _dir LIST_DIRECTORIES true "${_bin_dir}/../lib/clang/*/lib/windows")
  set(${out_var} "${_dir}" PARENT_SCOPE)
endfunction()

# enable_asan(<target>)
# AddressSanitizer with platform translation:
#   cl / clang-cl : /fsanitize=address. ASan does not support the debug CRT
#                   (/MDd), so the target is switched to the release CRT.
#                   clang-cl additionally links the ASan runtime explicitly
#                   (CMake drives Windows links through lld-link, not through
#                   the clang driver) and stages the runtime DLL next to the
#                   binary.
#   gcc / clang   : -fsanitize=address on both compile and link.
# NOTE: apply to concrete targets; an interface target would only propagate
#       the compile flag, leaving consumers without the runtime at link time.
function(enable_asan target)
  codebase_option_scope(${target} _scope)
  if(MSVC)
    # ASan does not support the debug CRT (/MDd), and a sanitized binary
    # must not mix CRTs with the unsanitized libraries it links (base,
    # gtest). Under a multi-config generator everything below is therefore
    # guarded to non-Debug configs; single-config generators keep the
    # unconditional behavior (the config is known at configure time).
    if(CMAKE_CONFIGURATION_TYPES)
      set(_asan_config_guard "$<NOT:$<CONFIG:Debug>>")
      # The CRT property must spell out BOTH branches: an empty expansion in
      # Debug would suppress the flag entirely, and clang-cl then defaults to
      # the release CRT — a silent ABI mismatch with the debug-CRT libraries
      # (gtest) that crashes at startup instead of failing the link.
      set(_asan_crt "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    else()
      set(_asan_config_guard "1")
      set(_asan_crt "MultiThreadedDLL")
    endif()
    target_compile_options(${target} ${_scope} "$<${_asan_config_guard}:/fsanitize=address>")
    # MSVC STL embeds /failifmismatch pragmas for its container annotations
    # (string/vector/optional), whose values differ between sanitized and
    # unsanitized TUs. Since the ASan target links unsanitized objects
    # (vcpkg gtest, base, ...), disable the annotations or lld-link rejects
    # the mix. _DISABLE_STL_ANNOTATION is the master switch (see
    # __msvc_sanitizer_annotate_container.hpp).
    target_compile_definitions(${target} ${_scope} "$<${_asan_config_guard}:_DISABLE_STL_ANNOTATION>")
    get_target_property(_type ${target} TYPE)
    if(NOT _type STREQUAL "INTERFACE_LIBRARY")
      set_property(TARGET ${target} PROPERTY MSVC_RUNTIME_LIBRARY "${_asan_crt}")
      if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        codebase_clang_rt_windows_dir(_rt_dir)
        if(_rt_dir)
          if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(_arch "x86_64")
          else()
            set(_arch "i386")
          endif()
          target_link_directories(${target} ${_scope} "${_rt_dir}")
          target_link_libraries(${target} ${_scope}
            "$<${_asan_config_guard}:clang_rt.asan_dynamic-${_arch}>"
            "$<${_asan_config_guard}:-WHOLEARCHIVE:clang_rt.asan_dynamic_runtime_thunk-${_arch}.lib>")
          # The ASan runtime DLL must sit next to the executable at run time.
          add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
              "${_rt_dir}/clang_rt.asan_dynamic-${_arch}.dll"
              "$<TARGET_FILE_DIR:${target}>")
        else()
          message(WARNING
            "enable_asan(${target}): Clang runtime directory not found; "
            "the ASan runtime must be linked manually.")
        endif()
      endif()
    endif()
  else()
    target_compile_options(${target} ${_scope} "-fsanitize=address" "-fno-omit-frame-pointer")
    target_link_options(${target} ${_scope} "-fsanitize=address")
  endif()
endfunction()

# enable_tsan(<target>)
# ThreadSanitizer: -fsanitize=thread on both compile and link. There is no
# TSan runtime for Windows, so this is a (loud) no-op there.
# NOTE: TSan is mutually exclusive with ASan in the same binary; give TSan
# targets their own executable rather than stacking enable_xxx calls.
function(enable_tsan target)
  codebase_option_scope(${target} _scope)
  if(MSVC)
    message(WARNING
      "enable_tsan(${target}): TSan has no Windows runtime; the target is "
      "built unsanitized. Run it on Linux for actual race detection.")
    return()
  endif()
  target_compile_options(${target} ${_scope} "-fsanitize=thread" "-fno-omit-frame-pointer")
  target_link_options(${target} ${_scope} "-fsanitize=thread")
endfunction()
