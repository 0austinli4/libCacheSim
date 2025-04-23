cmake_minimum_required(VERSION 3.12)

project(pylibCacheSim LANGUAGES CXX)

# Find Python and pybind11
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 REQUIRED)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/libCacheSim/include
    /home/al2926/.local/include
)

# Library directories
link_directories(
    ${CMAKE_SOURCE_DIR}/_build
    /home/al2926/.local/lib
)

# Add the Python extension module
pybind11_add_module(pylibCacheSim src/CacheWrapper.cpp)

# Find and link GLib (for g_direct_hash)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)
target_include_directories(pylibCacheSim PRIVATE ${GLIB_INCLUDE_DIRS})
target_link_libraries(pylibCacheSim PRIVATE ${GLIB_LIBRARIES})

# Link against CacheSim
# If the library is not found automatically, specify the full path
set(CACHESIM_LIB "CacheSim")
target_link_libraries(pylibCacheSim PRIVATE ${CACHESIM_LIB})

# For runtime library search path (rpath)
set_target_properties(pylibCacheSim PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    INSTALL_RPATH "${CMAKE_SOURCE_DIR}/_build;/home/al2926/.local/lib"
)

# Set C++ standard
set_target_properties(pylibCacheSim PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
