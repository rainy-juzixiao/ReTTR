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

function(rettr_fetch_import_std_uuid)
    find_package(Python3 QUIET COMPONENTS Interpreter)

    if(NOT Python3_FOUND)
        message(WARNING "rettr_fetch_import_std_uuid: Python3 not found, "
                        "CMAKE_EXPERIMENTAL_CXX_IMPORT_STD will not be set")
        return()
    endif()

    set(_SCRIPT "${CMAKE_SOURCE_DIR}/scripts/fetch_cmake_import_std_module_uid.py")

    if(NOT EXISTS "${_SCRIPT}")
        message(WARNING "rettr_fetch_import_std_uuid: script not found at ${_SCRIPT}, "
                        "CMAKE_EXPERIMENTAL_CXX_IMPORT_STD will not be set")
        return()
    endif()

    execute_process(
        COMMAND "${Python3_EXECUTABLE}" "${_SCRIPT}" -q
        OUTPUT_VARIABLE _IMPORT_STD_UUID
        ERROR_VARIABLE _IMPORT_STD_ERROR
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE _IMPORT_STD_RESULT
        TIMEOUT 30
    )

    if(_IMPORT_STD_RESULT EQUAL 0 AND _IMPORT_STD_UUID)
        message(STATUS "Auto-detected CMAKE_EXPERIMENTAL_CXX_IMPORT_STD: ${_IMPORT_STD_UUID}")
        set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "${_IMPORT_STD_UUID}" PARENT_SCOPE)
    else()
        message(WARNING "rettr_fetch_import_std_uuid: script failed (${_IMPORT_STD_RESULT}): "
                        "${_IMPORT_STD_ERROR}, "
                        "CMAKE_EXPERIMENTAL_CXX_IMPORT_STD will not be set")
    endif()
endfunction()
