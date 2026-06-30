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
include(CheckCXXSourceCompiles)

function(rettr_has_module_cmake_support result_var)
    if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.28")
        set(${result_var} TRUE PARENT_SCOPE)
    else()
        set(${result_var} FALSE PARENT_SCOPE)
    endif()
endfunction()

function(rettr_has_compiler_module_support result_var)
    if(MSVC)
        # MSVC modules support: VS 2022 17.5+ (_MSC_VER >= 1935)
        if(MSVC_VERSION GREATER_EQUAL 1935)
            set(${result_var} TRUE PARENT_SCOPE)
        else()
            set(${result_var} FALSE PARENT_SCOPE)
        endif()
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT MSVC)
        # Clang modules support: 17+
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "17.0.0")
            set(${result_var} TRUE PARENT_SCOPE)
        else()
            set(${result_var} FALSE PARENT_SCOPE)
        endif()
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        # GCC modules support: 14+
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "14.0.0")
            set(${result_var} TRUE PARENT_SCOPE)
        else()
            set(${result_var} FALSE PARENT_SCOPE)
        endif()
    else()
        set(${result_var} FALSE PARENT_SCOPE)
    endif()
endfunction()

function(rettr_check_actual_module_compile result_var)
    set(CMAKE_REQUIRED_FLAGS "-std=c++20")
    set(_module_test_src [=[
        module;
        #include <cstdint>
        export module _rettr_module_test;
        export int _rettr_module_fn() { return 42; }
    ]=])
    check_cxx_source_compiles("${_module_test_src}" ${result_var})
    set(${result_var} ${${result_var}} PARENT_SCOPE)
endfunction()

function(rettr_setup_import_std uuid)
    if(NOT uuid STREQUAL "")
        set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD
            "${uuid}"
            CACHE STRING "UUID for experimental import std support"
        )
        message(STATUS "CMAKE_EXPERIMENTAL_CXX_IMPORT_STD configured with UUID: ${uuid}")
    else()
        message(STATUS "RETTR_IMPORT_STD_UUID is ON but no UUID provided, skipping import std setup")
    endif()
endfunction()

macro(rettr_add_module_target target_name)
    add_library(${target_name})
    set_target_properties(${target_name} PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
    )
    target_sources(${target_name}
        PUBLIC
            FILE_SET CXX_MODULES
            BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
            FILES ${ARGN}
    )
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        target_compile_options(${target_name} PRIVATE
            -fmodules-ts
            -Wno-expose-global-module-tu-local
        )
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        if(MSVC)
            target_compile_options(${target_name} PRIVATE /experimental:module)
        else()
            target_compile_options(${target_name} PRIVATE -fmodules-ts)
        endif()
    elseif(MSVC)
        target_compile_options(${target_name} PRIVATE /experimental:module /std:c++20)
    endif()

    target_include_directories(${target_name}
        PUBLIC
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/lunar/include>
            $<INSTALL_INTERFACE:include>
    )
    target_compile_definitions(${target_name}
        PRIVATE
            RETTR_DYNAMIC_EXPORTS=1
            RETTR_PROJECT_VERSION="${PROJECT_VERSION}"
            RETTR_PROJECT_MAJOR=${PROJECT_VERSION_MAJOR}
            RETTR_PROJECT_MINOR=${PROJECT_VERSION_MINOR}
            RETTR_PROJECT_PATCH=${PROJECT_VERSION_PATCH}
    )
    target_compile_definitions(${target_name}
        PUBLIC
            RETTR_USING_DYNAMIC=1
    )
endmacro()

function(rettr_apply_module_flags target)
    set_target_properties(${target} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        target_compile_options(${target} PRIVATE -fmodules-ts)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT MSVC)
        target_compile_options(${target} PRIVATE -fmodules-ts)
    elseif(MSVC)
        target_compile_options(${target} PRIVATE /experimental:module /std:c++20)
    endif()

    target_link_libraries(${target} PRIVATE rettr_module rettr)
endfunction()

function(rettr_module_support_summary result_var)
    set(_parts "")

    string(TIMESTAMP _now)

    list(APPEND _parts "ReTTR Module Build Support (generated ${_now})")
    list(APPEND _parts "  CMake version:           ${CMAKE_VERSION}")
    list(APPEND _parts "  Compiler:                ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
    list(APPEND _parts "  C++ standard:            ${CMAKE_CXX_STANDARD}")

    rettr_has_module_cmake_support(_has_cmake)
    rettr_has_compiler_module_support(_has_comp)
    rettr_check_actual_module_compile(_has_actual)

    list(APPEND _parts "  CMake module support:    ${_has_cmake}")
    list(APPEND _parts "  Compiler module support: ${_has_comp}")
    list(APPEND _parts "  Actual compile check:    ${_has_actual}")
    list(APPEND _parts "  import std UUID:         ${CMAKE_EXPERIMENTAL_CXX_IMPORT_STD}")

    string(REPLACE ";" "\n" ${result_var} "${_parts}")
    set(${result_var} "${${result_var}}" PARENT_SCOPE)
endfunction()
