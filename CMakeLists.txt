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

project(ReTTR VERSION ${VERSION} LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(RETTR_BUILD_WITH_DYNAMIC "This option allows you build rettr as a dynamic libraray." on)
option(RETTR_BUILD_EXAMPLES "Build the example programs bundled with ReTTR." on)
option(RETTR_BUILD_UNIT_TESTS "This option allows building unit tests." on)
option(RETTR_USE_CROSSCOMPILE "Enable for cross compile." off)
option(RETTR_USING_UTF8_INPUT_FOR_MSVC "Use utf-8 encoding for input. Reason: we need avoid mutilangauge problems." on)
option(RETTR_USING_UTF8_OUTPUT_FOR_MSVC "Use utf-8 encoding for output. Reason: we need avoid mutilangauge problems." on)
option(RETTR_USE_CXX26_REFLECTION_TS "Allows rettr to use c++ 26 reflection ts features." on)

include(${PROJECT_SOURCE_DIR}/cmake/utility.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/config.cmake)

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
endif ()

include(${PROJECT_SOURCE_DIR}/cmake/install.cmake)