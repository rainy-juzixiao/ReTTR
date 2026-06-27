#                                 Apache License
#                           Version 2.0, January 2004
#                        http://www.apache.org/licenses/
#
#   Copyright 2026 rainy-juzixiao
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# POSIX (Linux / macOS): honour the user's CMAKE_INSTALL_PREFIX, defaulting to
# /usr/local.  Headers go to <prefix>/include/rettr, libraries to <prefix>/lib,
# CMake package files to <prefix>/lib/cmake/ReTTR.
#
# Windows: force the install tree into the build directory so that the result
# is self-contained and doesn't require admin rights.  The layout mirrors the
# POSIX one underneath <build-dir>/install.
if(WIN32)
    set(RETTR_DEFAULT_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")
    if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        set(CMAKE_INSTALL_PREFIX "${RETTR_DEFAULT_INSTALL_PREFIX}"
            CACHE PATH "ReTTR install prefix (defaults to build directory on Windows)" FORCE)
    endif()
    message(STATUS "[ReTTR] Windows detected — install tree will be placed in ${CMAKE_INSTALL_PREFIX}")
else()
    message(STATUS "[ReTTR] POSIX detected — installing to ${CMAKE_INSTALL_PREFIX}")
endif()

install(TARGETS rettr
    EXPORT  ReTTRTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"   # .a  / .lib
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"   # .so / .dylib
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"   # .dll
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

# Install public headers (preserving directory structure under rettr/)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/lunar/include/rettr"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT devel
    FILES_MATCHING
    PATTERN "*.hpp"
    PATTERN "*.h"
)

# Export targets for the build tree (so build-tree usage works too)
export(EXPORT ReTTRTargets
    FILE "${CMAKE_BINARY_DIR}/cmake/ReTTRTargets.cmake"
    NAMESPACE RETTR::
)

# Write the version file
write_basic_package_version_file(
    "${CMAKE_BINARY_DIR}/cmake/ReTTRConfigVersion.cmake"
    VERSION       "${PROJECT_VERSION}"
    COMPATIBILITY SameMajorVersion
)

# Configure the package-config template
set(RETTR_CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/ReTTRConfig.cmake.in"
    "${CMAKE_BINARY_DIR}/cmake/ReTTRConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/ReTTR"
    PATH_VARS          CMAKE_INSTALL_INCLUDEDIR
)

# Install the generated CMake package files
install(FILES
    "${CMAKE_BINARY_DIR}/cmake/ReTTRConfig.cmake"
    "${CMAKE_BINARY_DIR}/cmake/ReTTRConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/ReTTR"
    COMPONENT devel
)

# Install exported targets (install-tree)
install(EXPORT ReTTRTargets
    FILE       ReTTRTargets.cmake
    NAMESPACE  RETTR::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/ReTTR"
    COMPONENT  devel
)

# Install the version file alongside the package
install(FILES "${PROJECT_SOURCE_DIR}/version.txt"
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/rettr"
    COMPONENT devel
    OPTIONAL
)

# Summary
message(STATUS "[ReTTR] Install configuration complete.")
message(STATUS "  Headers  → ${CMAKE_INSTALL_FULL_INCLUDEDIR}/rettr")
message(STATUS "  Library  → ${CMAKE_INSTALL_FULL_LIBDIR}")
if(WIN32)
    message(STATUS "  Runtime  → ${CMAKE_INSTALL_FULL_BINDIR}")
endif()
message(STATUS "  CMake    → ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/ReTTR")
message(STATUS "  Usage:     find_package(ReTTR REQUIRED)")
message(STATUS "             target_link_libraries(<your-target> RETTR::rettr)")
