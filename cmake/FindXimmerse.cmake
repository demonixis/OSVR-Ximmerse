# - try to find the Ximmerse SDK
#
# IMPORTED Targets:
#
# This module defines :prop_tgt:`IMPORTED` target ``Ximmerse::Ximmerse``, if
# the Ximmerse SDK has been found.
#
# Non-cache variables you might use in your CMakeLists.txt:
#  XIMMERSE_FOUND
#  XIMMERSE_INCLUDE_DIRS
#  XIMMERSE_LIBRARIES
#
# Cache Variables: (probably not for direct use in your scripts)
#  XIMMERSE_INCLUDE_DIR
#  XIMMERSE_XDEVICE_LIBRARY
#  XIMMERSE_INTERPOLATION_LIBRARY
#
# Requires these CMake modules:
#  FindPackageHandleStandardArgs (known included with CMake >=2.6.2)
#
# Original Author:
# 2017 Kevin M. Godby <kevin@godby.org>
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

set(XIMMERSE_ROOT_DIR
	"${XIMMERSE_ROOT_DIR}"
	CACHE
	PATH
	"Directory to search for Ximmerse SDK")

set(XIMMERSE_HEADERS_ROOT_DIR
	"${XIMMERSE_HEADERS_ROOT_DIR}"
	CACHE
	PATH
	"Directory to search for private Ximmerse headers")

set(_root_dirs)
if(XIMMERSE_ROOT_DIR)
	set(_root_dirs "${XIMMERSE_ROOT_DIR}" "${XIMMERSE_HEADERS_ROOT_DIR}" "${XIMMERSE_ROOT_DIR}/public")
endif()

# todo fails for universal builds
set(_dll_suffix)
if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
	set(_bitness 64)
	if(WIN32)
		set(_dll_suffix _x64)
	endif()
else()
	set(_bitness 32)
endif()

# Test platform

set(_platform)
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(_platform_base osx)
	# SteamVR only supports 32-bit on OS X
	set(XIMMERSE_PLATFORM osx32)
else()
	if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		set(_platform_base linux)
		# TODO Massive hack!
		add_definitions(-DGNUC -DPOSIX -DCOMPILER_GCC -D_LINUX -DLINUX -DPOSIX -D_POSIX)
	elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
		set(_platform_base win)
	endif()
	set(XIMMERSE_PLATFORM ${_platform_base}${_bitness})
	set(_libpath lib/${XIMMERSE_PLATFORM})
endif()

find_path(XIMMERSE_INCLUDE_DIR
	NAMES
	xdevice.h
	HINTS
	"${CMAKE_CURRENT_SOURCE_DIR}/Vendors/Ximmerse/include"
	PATHS
	${_root_dirs})

find_library(XIMMERSE_XDEVICE_LIBRARY
	NAMES
	xdevice
	HINTS
	"${CMAKE_CURRENT_SOURCE_DIR}/Vendors/Ximmerse/libs"
	PATH_SUFFIXES
	x86
	x64)

find_library(XIMMERSE_INTERPOLATION_LIBRARY
	NAMES
	interpolation
	HINTS
	"${CMAKE_CURRENT_SOURCE_DIR}/Vendors/Ximmerse/libs"
	PATH_SUFFIXES
	x86
	x64)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Ximmerse
	DEFAULT_MSG
	XIMMERSE_INCLUDE_DIR
	XIMMERSE_XDEVICE_LIBRARY
	XIMMERSE_INTERPOLATION_LIBRARY)

if(XIMMERSE_FOUND)
	list(APPEND XIMMERSE_INCLUDE_DIRS ${XIMMERSE_INCLUDE_DIR})
	list(APPEND XIMMERSE_LIBRARIES ${XIMMERSE_XDEVICE_LIBRARY} ${XIMMERSE_INTERPOLATION_LIBRARY})
	mark_as_advanced(XIMMERSE_ROOT_DIR)
endif()

mark_as_advanced(XIMMERSE_INCLUDE_DIR XIMMERSE_XDEVICE_LIBRARY XIMMERSE_INTERPOLATION_LIBRARY)

if(Ximmerse_FOUND AND NOT TARGET Ximmerse::Ximmerse)
	add_library(Ximmerse::Ximmerse UNKNOWN IMPORTED)
	set_target_properties(Ximmerse::Ximmerse PROPERTIES
		IMPORTED_LOCATION "${Ximmerse_LIBRARIES}"
		INTERFACE_INCLUDE_DIRECTORIES "${Ximmerse_INCLUDE_DIRS}")
endif()

