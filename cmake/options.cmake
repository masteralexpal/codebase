# cmake/options.cmake
# -------------------
# Build options declared by this project, toggled by the builder with
# cmake -D<OPTION>=ON/OFF (or via presets). Options are prefixed CODEBASE_ to
# avoid collisions when this project is embedded into a larger build.
# Implementations of the corresponding enable_xxx(<target>) functions live in
# their own feature modules under cmake/.
# -------------------
include_guard(GLOBAL)

# Link-time optimization for first-party targets (implemented in lto.cmake).
option(CODEBASE_ENABLE_LTO "Enable link-time optimization (LTO/IPO)" OFF)

# Component build (Chromium-style): ON builds libraries as shared components,
# OFF builds static libraries. Drives the *_STATIC_DEFINE / *_IMPLEMENTATION
# export macros (see base/base_exports.h).
option(CODEBASE_COMPONENT_BUILD "Build libraries as shared components" OFF)
