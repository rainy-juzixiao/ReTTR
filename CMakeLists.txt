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

cmake_minimum_required(VERSION 3.22.1)

file(READ "${CMAKE_SOURCE_DIR}/version.txt" VERSION)
string(STRIP "${VERSION}" VERSION)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(RETTR_USE_MODULE_BUILD "This option is used for enable module build support." off)
option(RETTR_BUILD_WITH_DYNAMIC "This option allows you build rettr as a dynamic libraray." on)
option(RETTR_BUILD_EXAMPLES "Build the example programs bundled with ReTTR." on)
option(RETTR_BUILD_UNIT_TESTS "This option allows building unit tests." on)
option(RETTR_USE_CROSSCOMPILE "Enable for cross compile." off)
option(RETTR_USING_UTF8_INPUT_FOR_MSVC "Use utf-8 encoding for input. Reason: we need avoid mutilangauge problems." on)
option(RETTR_USING_UTF8_OUTPUT_FOR_MSVC "Use utf-8 encoding for output. Reason: we need avoid mutilangauge problems." on)
option(RETTR_USE_CXX26_REFLECTION_TS "Allows rettr to use c++ 26 reflection ts features." off)

if (NOT CMAKE_EXPERIMENTAL_CXX_IMPORT_STD STREQUAL "" AND RETTR_USE_MODULE_BUILD)
    set(CMAKE_CXX_STANDARD 23)

    include(${CMAKE_SOURCE_DIR}/cmake/fetch_import_std.cmake)
    rettr_fetch_import_std_uuid()

    set(CMAKE_CXX_MODULE_STD ON)
    set(CMAKE_CXX_SCAN_FOR_MODULES ON)

    set(RETTR_HAS_MODULE_SUPPORT TRUE)
else ()
    set(CMAKE_CXX_STANDARD 17)
    set(RETTR_HAS_MODULE_SUPPORT FALSE)
endif ()

project(ReTTR VERSION ${VERSION} LANGUAGES CXX C)

include(${PROJECT_SOURCE_DIR}/cmake/utility.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/config.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/module_build.cmake)

# GCC is NOT supported for module builds in ReTTR
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND RETTR_USE_MODULE_BUILD)
    if(RETTR_USE_CXX26_REFLECTION_TS)
        message(FATAL_ERROR
            "GCC does NOT support C++20 modules together with C++26 Reflection TS.\n"
            "Please choose one:\n"
            "  - Set RETTR_USE_CXX26_REFLECTION_TS=OFF to use module build, OR\n"
            "  - Set RETTR_USE_MODULE_BUILD=OFF to use reflection")
    endif()
    message(FATAL_ERROR
        "GCC is not supported for C++20 module builds in ReTTR.\n"
        "Please use Clang 17+ or MSVC 2022 17.5+ instead,\n"
        "or set RETTR_USE_MODULE_BUILD=OFF to build without modules.")
endif()

if (RETTR_HAS_MODULE_SUPPORT)
    add_subdirectory(${PROJECT_SOURCE_DIR}/lunar/modules)
endif ()

if (RETTR_BUILD_EXAMPLES)
    add_subdirectory(${PROJECT_SOURCE_DIR}/lunar/examples)
endif ()

if (RETTR_BUILD_UNIT_TESTS)
    enable_testing()
    add_subdirectory(${PROJECT_SOURCE_DIR}/third_party)
    add_subdirectory(${PROJECT_SOURCE_DIR}/lunar/tests)
    add_subdirectory(${PROJECT_SOURCE_DIR}/lunar/benchmarks)

    add_executable(test_program ${PROJECT_SOURCE_DIR}/test_programs/main.cpp)
    target_link_libraries(test_program rettr)

    if(RETTR_HAS_MODULE_SUPPORT)
        add_executable(module_test_program)

        target_sources(module_test_program
                PRIVATE
                FILE_SET CXX_MODULES
                FILES
                misc/modules_test/main.cppm
        )

        set_target_properties(module_test_program PROPERTIES CXX_STANDARD 23)
        target_link_libraries(module_test_program PRIVATE rettr_module)
        target_compile_definitions(module_test_program PRIVATE RETTR_HAS_MODULE_SUPPORT=1)
   endif()
endif ()

include(${PROJECT_SOURCE_DIR}/cmake/install.cmake)