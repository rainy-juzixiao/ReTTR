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

project(plugin_example LANGUAGES CXX)

add_library(plugin_example SHARED "main.cpp")

target_link_libraries(plugin_example rainy-juzixiao::rettr)

set_target_properties(plugin_example PROPERTIES
        DEBUG_POSTFIX "_d"
        "Examples/library_loading"
        CXX_STANDARD ${MAX_CXX_STANDARD}
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if (CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(plugin_example PRIVATE "-fno-gnu-unique") # 不要省略该编译选项！否则，卸载机制将会失效
endif()
