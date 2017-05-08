cmake_minimum_required(VERSION 2.8.12)
project(OsvrXimmerse)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(osvr REQUIRED)
find_package(Ximmerse REQUIRED)

# This generates a header file, from the named json file, containing a string literal
# named net_demonixis_ximmerse_json_json (not null terminated)
# The file must be added as a source file to some target (as below) to be generated.
osvr_convert_json(net_demonixis_ximmerse_json
    net_demonixis_ximmerse.json
    "${CMAKE_CURRENT_BINARY_DIR}/net_demonixis_ximmerse_json.h")

# This is just a helper function wrapping CMake's add_library command that
# sets up include dirs, libraries, and naming convention (no leading "lib")
# for an OSVR plugin. It also installs the plugin into the right directory.
# Pass as many source files as you need. See osvrAddPlugin.cmake for full docs.
osvr_add_plugin(NAME net_demonixis_ximmerse
    CPP # indicates we'd like to use the C++ wrapper
    SOURCES
    net_demonixis_ximmerse.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/net_demonixis_ximmerse_json.h")

# Be able to find our generated header file.
target_include_directories(net_demonixis_ximmerse
	PUBLIC
	"${CMAKE_CURRENT_BINARY_DIR}"
)

# If you use other libraries, find them and add a line like:
target_link_libraries(net_demonixis_ximmerse
	Ximmerse::Ximmerse
)

# Install Ximmerse libraries to osvr_server directory
include(CopyImportedTarget)
include(GNUInstallDirs)
get_target_property(_targets Ximmerse::Ximmerse INTERFACE_LINK_LIBRARIES)
list(APPEND _targets Ximmerse::WCL04)
foreach(_target IN LISTS _targets)
	copy_imported_targets(${_target}) # for use in build tree
	install_imported_target(${_target} DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT Runtime) # in installed directory
endforeach()
