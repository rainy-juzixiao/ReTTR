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

project(bench_method LANGUAGES CXX)

add_executable(bench_method
        "main.cpp"
        "methods.cpp"
        "find_method.cpp"
        "invoke_method.cpp"
)

target_link_libraries(bench_method rainy-juzixiao::rettr)
target_link_libraries(bench_method nanobench::nanobench)

set_target_properties(bench_method PROPERTIES
        DEBUG_POSTFIX ${rettr_debug_postfix}
        FOLDER "Benchmarks"
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/rettr_project
)

if (COMPILER_ID MATCHES "MSVC")
    target_compile_options(bench_method PRIVATE /bigobj) # 确保可以被编译
endif ()