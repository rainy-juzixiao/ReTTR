rettr_load_flodar_files("." ".cpp" RETTR_UNIT_TEST_LIST)

add_executable(rettr-unit-test-program ${RETTR_UNIT_TEST_LIST})

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/third_party/catch2/extras)

include(Catch)
catch_discover_tests(rettr-unit-test-program)

target_link_libraries(rettr-unit-test-program PRIVATE
	rainy-juzixiao::rettr
	Catch2::Catch2WithMain
)

target_include_directories(rettr-unit-test-program PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(rettr-unit-test-program PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/rettr_project
)