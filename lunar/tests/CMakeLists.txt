rettr_load_flodar_files("." ".cpp" RETTR_UNIT_TEST_LIST)

add_executable(rettr-unit-test-program ${RETTR_UNIT_TEST_LIST})

target_link_libraries(rettr-unit-test-program PRIVATE
	rainy-juzixiao::rettr
	Catch2::Catch2WithMain
)