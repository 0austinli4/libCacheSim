cmake_minimum_required(VERSION 3.14)
project(pylibCacheSim)

# Find required packages
find_package(pybind11 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)

# Set include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../libCacheSim/include
    ${pybind11_INCLUDE_DIRS}
    /opt/homebrew/Cellar/glib/2.84.1/include/glib-2.0
    /opt/homebrew/Cellar/glib/2.84.1/lib/glib-2.0/include
)

# Add libCacheSim library
add_library(libCacheSim STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../libCacheSim/cache/cache.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../libCacheSim/cache/eviction/FIFO.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../libCacheSim/cache/eviction/FIFO_Reinsertion.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../libCacheSim/cache/eviction/LFU.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../libCacheSim/cache/eviction/LRU.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../libCacheSim/cache/eviction/S3FIFO.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../libCacheSim/cache/eviction/Sieve.c
)

# Create the Python module
pybind11_add_module(pylibCacheSim
    src/CacheWrapper.cpp
    src/pybind11_bindings.cpp
)

# Set rpath for macOS
set_target_properties(pylibCacheSim PROPERTIES
    INSTALL_RPATH "@loader_path"
    BUILD_WITH_INSTALL_RPATH TRUE
)

# Link against required libraries
target_link_libraries(pylibCacheSim PRIVATE
    libCacheSim
    pybind11::module
    /opt/homebrew/Cellar/glib/2.84.1/lib/libglib-2.0.0.dylib
    /opt/homebrew/Cellar/glib/2.84.1/lib/libgobject-2.0.0.dylib
    /opt/homebrew/Cellar/glib/2.84.1/lib/libgmodule-2.0.0.dylib
    /opt/homebrew/Cellar/glib/2.84.1/lib/libgio-2.0.0.dylib
    /opt/homebrew/Cellar/glib/2.84.1/lib/libgthread-2.0.0.dylib
)

# Add compiler flags
target_compile_options(pylibCacheSim PRIVATE
    ${GLIB_CFLAGS}
)
