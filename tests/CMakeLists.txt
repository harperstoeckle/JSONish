add_executable(jsonish-tests
	main.test.cpp
	lex.test.cpp
	parse.test.cpp)

target_link_libraries(jsonish-tests
	PRIVATE
	jsonish)

target_include_directories(jsonish-tests
	PRIVATE
	${PROJECT_SOURCE_DIR}/src)

find_package(Catch2)
target_link_libraries(jsonish-tests
	PRIVATE
	Catch2::Catch2)
