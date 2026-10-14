# cmake/compiler.cmake
# --------------------
# Compiler policy. Only the supported toolchains are accepted, anything else
# is rejected at configure time with a descriptive error:
#
#   Linux   : GCC, Clang
#   macOS   : Clang (AppleClang or upstream LLVM Clang)
#   Windows : ClangCL only (clang with the MSVC-like frontend; plain MSVC cl
#             and MinGW/GNU-frontend compilers are rejected)
# --------------------

# codebase_check_compiler()
# Validates the platform and every enabled language's compiler. Call right
# after project(), when the compiler id variables are available.
function(codebase_check_compiler)
  if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(_supported_ids "Clang")
    set(_required_frontend "MSVC")
    set(_policy "ClangCL")
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(_supported_ids "GNU" "Clang")
    set(_policy "GCC, Clang")
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(_supported_ids "AppleClang" "Clang")
    set(_policy "Clang")
  else()
    message(FATAL_ERROR
      "[codebase] Unsupported platform: ${CMAKE_SYSTEM_NAME}. "
      "Supported platforms: Windows, Linux, macOS.")
  endif()

  foreach(_lang C CXX)
    set(_id "${CMAKE_${_lang}_COMPILER_ID}")
    if(NOT _id IN_LIST _supported_ids)
      message(FATAL_ERROR
        "[codebase] Unsupported ${_lang} compiler '${_id}' on "
        "${CMAKE_SYSTEM_NAME}. Supported: ${_policy}.")
    endif()
    if(DEFINED _required_frontend)
      set(_frontend "${CMAKE_${_lang}_COMPILER_FRONTEND_VARIANT}")
      if(NOT _frontend STREQUAL _required_frontend)
        message(FATAL_ERROR
          "[codebase] Unsupported ${_lang} compiler frontend '${_frontend}' "
          "on Windows. Only ClangCL (clang with the MSVC-like frontend) is "
          "supported; plain MSVC cl and GNU-frontend compilers are not.")
      endif()
    endif()
    message(STATUS "[codebase] ${_lang} compiler check passed: ${_id} "
      "${CMAKE_${_lang}_COMPILER_VERSION}")
  endforeach()
endfunction()
