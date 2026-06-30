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

include(${PROJECT_SOURCE_DIR}/cmake/module_build.cmake)

rettr_has_module_cmake_support(RETTR_MODULE_CMAKE_OK)
rettr_has_compiler_module_support(RETTR_MODULE_COMPILER_OK)
rettr_check_actual_module_compile(RETTR_MODULE_ACTUAL_OK)

set(RETTR_CAN_BUILD_MODULES FALSE)
if (RETTR_MODULE_CMAKE_OK AND RETTR_MODULE_COMPILER_OK)
    set(RETTR_CAN_BUILD_MODULES TRUE)
endif ()

if (RETTR_IMPORT_STD_UUID AND DEFINED RETTR_IMPORT_STD_UUID_VALUE)
    rettr_setup_import_std("${RETTR_IMPORT_STD_UUID_VALUE}")
endif ()

if (RETTR_CAN_BUILD_MODULES)
    message(STATUS "C++20 modules available — building rettr module targets")
    set(RETTR_HAS_MODULE_SUPPORT TRUE PARENT_SCOPE)

    rettr_add_module_target(rettr_module
            rettr/any.cppm
            rettr/registration.cppm
    )

    set_target_properties(rettr_module PROPERTIES
            OUTPUT_NAME "rettr"
            POSITION_INDEPENDENT_CODE ON
    )

    add_library(RETTR::rettr_module ALIAS rettr_module)
    add_library(rainy-juzixiao::rettr_module ALIAS rettr_module)

    message(STATUS "rettr_module target created — use `import rettr;` in your C++20 projects")

    target_compile_definitions(rettr PUBLIC RETTR_HAS_MODULE_SUPPORT=1)
    target_link_libraries(rettr_module PRIVATE rettr)

else ()
    message(STATUS "C++20 module support not detected, skipping module target")
    message(STATUS "  Requirements: CMake >= 3.28, Clang 17+ / MSVC 2022 17.5+")

    set(RETTR_HAS_MODULE_SUPPORT FALSE PARENT_SCOPE)

    add_library(rettr_module INTERFACE)
    target_link_libraries(rettr_module INTERFACE rettr)
    add_library(RETTR::rettr_module ALIAS rettr_module)
    add_library(rainy-juzixiao::rettr_module ALIAS rettr_module)

    target_compile_definitions(rettr PUBLIC RETTR_HAS_MODULE_SUPPORT=0)
endif ()
