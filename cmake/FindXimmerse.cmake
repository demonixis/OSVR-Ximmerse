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

find_path(XIMMERSE_INCLUDE_DIR
	NAMES
	xdevice.h
	HINTS
	"${CMAKE_CURRENT_SOURCE_DIR}/Vendors/Ximmerse/include"
	PATHS
	${XIMMERSE_ROOT_DIR})

find_library(XIMMERSE_XDEVICE_LIBRARY
	NAMES
	xdevice
	HINTS
	"${CMAKE_CURRENT_SOURCE_DIR}/Vendors/Ximmerse/libs"
	PATHS
	${XIMMERSE_ROOT_DIR}
	PATH_SUFFIXES
	x86
	x64)

find_file(XIMMERSE_WCL04_LIBRARY
	NAMES
	"${CMAKE_SHARED_LIBRARY_PREFIX}wcl04${CMAKE_SHARED_LIBRARY_SUFFIX}"
	wcl04
	HINTS
	"${CMAKE_CURRENT_SOURCE_DIR}/Vendors/Ximmerse/libs"
	PATHS
	${XIMMERSE_ROOT_DIR}
	PATH_SUFFIXES
	x86
	x64
	NO_DEFAULT_PATH)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Ximmerse
	DEFAULT_MSG
	XIMMERSE_INCLUDE_DIR
	XIMMERSE_XDEVICE_LIBRARY
	XIMMERSE_WCL04_LIBRARY)

if(XIMMERSE_FOUND)
	list(APPEND XIMMERSE_INCLUDE_DIRS ${XIMMERSE_INCLUDE_DIR})
	list(APPEND XIMMERSE_LIBRARIES ${XIMMERSE_XDEVICE_LIBRARY} ${XIMMERSE_WCL04_LIBRARY})
	mark_as_advanced(XIMMERSE_ROOT_DIR)

	# Find the associated .dll file
	if (WIN32)
		get_filename_component(XIMMERSE_XDEVICE_LIBDIR "${XIMMERSE_XDEVICE_LIBRARY}" PATH)
		get_filename_component(XIMMERSE_XDEVICE_BASENAME "${XIMMERSE_XDEVICE_LIBRARY}" NAME_WE)
		get_filename_component(XIMMERSE_XDEVICE_LIBDIR_BASE "${XIMMERSE_XDEVICE_LIBDIR}" PATH)
		find_file(XIMMERSE_XDEVICE_DLL
			"${CMAKE_SHARED_LIBRARY_PREFIX}${XIMMERSE_XDEVICE_BASENAME}${CMAKE_SHARED_LIBRARY_SUFFIX}"
			HINTS
			"${XIMMERSE_XDEVICE_LIBDIR_BASE}"
			PATH_SUFFIXES
			x86
			x64
			NO_DEFAULT_PATH
		)
		mark_as_advanced(XIMMERSE_XDEVICE_DLL)
		if(XIMMERSE_XDEVICE_DLL)
			add_library(Ximmerse::XDevice SHARED IMPORTED)
			set_property(TARGET Ximmerse::XDevice PROPERTY IMPORTED_IMPLIB "${XIMMERSE_XDEVICE_LIBRARY}")
			set_property(TARGET Ximmerse::XDevice PROPERTY IMPORTED_LOCATION "${XIMMERSE_XDEVICE_DLL}")
		else()
			add_library(Ximmerse::XDevice STATIC IMPORTED)
			set_property(TARGET Ximmerse::XDevice PROPERTY IMPORTED_LOCATION "${XIMMERSE_XDEVICE_LIBRARY}")
		endif()
	else() 
		add_library(Ximmerse::XDevice UNKNOWN IMPORTED)
		set_property(TARGET Ximmerse::XDevice PROPERTY IMPORTED_LOCATION "${XIMMERSE_XDEVICE_LIBRARY}")
	endif()
	set_property(TARGET Ximmerse::XDevice PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${XIMMERSE_INCLUDE_DIRS}")

	add_library(Ximmerse::WCL04 SHARED IMPORTED)
	set_property(TARGET Ximmerse::WCL04 PROPERTY IMPORTED_LOCATION "${XIMMERSE_WCL04_LIBRARY}")

	set(XIMMERSE_LIBRARIES Ximmerse::XDevice)# Ximmerse::WCL04)
	set(XIMMERSE_INCLUDE_DIRS "${XIMMERSE_INCLUDE_DIR}")

	add_library(Ximmerse::Ximmerse INTERFACE IMPORTED)
	set_target_properties(Ximmerse::Ximmerse
		PROPERTIES
		INTERFACE_LINK_LIBRARIES "${XIMMERSE_LIBRARIES}"
		INTERFACE_INCLUDE_DIRECTORIES "${XIMMERSE_INCLUDE_DIRS}")
endif()

mark_as_advanced(XIMMERSE_INCLUDE_DIR XIMMERSE_XDEVICE_LIBRARY XIMMERSE_WCL04_LIBRARY)


