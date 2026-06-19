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

project(library_loader_example LANGUAGES CXX)

add_executable(library_loader_example "main.cpp")

target_link_libraries(library_loader_example rainy-juzixiao::rettr)
add_dependencies(library_loader_example plugin_example)

set_target_properties(library_loader_example PROPERTIES
        DEBUG_POSTFIX ${rettr_debug_postfix}
        FOLDER "Examples"
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/rettr_project
)

add_dependencies(examples library_loader_example)
