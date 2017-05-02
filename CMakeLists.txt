cmake_minimum_required(VERSION 2.8.12)
project(OsvrXimmerse) # Change this line.

# This looks for an osvrConfig.cmake file - most of the time it can be
# autodetected but you might need to create/extend CMAKE_PREFIX_PATH to include something like
# C:/Users/Ryan/Desktop/build/OSVR-Core-vc12 or
# C:/Users/Ryan/Downloads/OSVR-Core-Snapshot-v0.1-406-gaa55515-build54-vs12-32bit
# in the CMake GUI or command line.
find_package(osvr REQUIRED)

# This generates a header file, from the named json file, containing a string literal
# named net_demonixis_ximmerse_json_json (not null terminated)
# The file must be added as a source file to some target (as below) to be generated.
osvr_convert_json(net_demonixis_ximmerse_json
    net_demonixis_ximmerse.json
    "${CMAKE_CURRENT_BINARY_DIR}/net_demonixis_ximmerse_json.h")

# Be able to find our generated header file.
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# This is just a helper function wrapping CMake's add_library command that
# sets up include dirs, libraries, and naming convention (no leading "lib")
# for an OSVR plugin. It also installs the plugin into the right directory.
# Pass as many source files as you need. See osvrAddPlugin.cmake for full docs.
osvr_add_plugin(NAME net_demonixis_ximmerse
    CPP # indicates we'd like to use the C++ wrapper
    SOURCES
    net_demonixis_ximmerse.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/net_demonixis_ximmerse_json.h")

# If you use other libraries, find them and add a line like:
# target_link_libraries(net_demonixis_ximmerse_json AnyOtherLibraries)