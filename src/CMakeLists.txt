add_library(jsonish
	jsonish/lex.cpp jsonish/lex.hpp
	${JSONISH_INCLUDE_DIR}/jsonish/tree.hpp
	${JSONISH_INCLUDE_DIR}/jsonish/result.hpp
	${JSONISH_INCLUDE_DIR}/jsonish/source_position.hpp
	jsonish/parse.cpp ${JSONISH_INCLUDE_DIR}/jsonish/parse.hpp)

set_target_properties(jsonish
	PROPERTIES
	CXX_EXTENSIONS OFF)

target_compile_features(jsonish
	PUBLIC
	cxx_std_17)

target_include_directories(jsonish
	PUBLIC
	${JSONISH_INCLUDE_DIR}
	PRIVATE
	${JSONISH_SRC_DIR})

option(JSONISH_ENABLE_WARNINGS "Enable maximal warnings for jsonish" ON)
if (JSONISH_ENABLE_WARNINGS)
	set(MSVC_WARNINGS /W4)
	set(GCC_WARNINGS -pedantic -Wall -Wextra -Wcast-align -Wcast-qual
		-Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2
		-Winit-self -Wlogical-op -Wmissing-declarations
		-Wmissing-include-dirs -Wnoexcept -Wold-style-cast
		-Woverloaded-virtual -Wredundant-decls -Wshadow
		-Wsign-conversion -Wsign-promo -Wstrict-null-sentinel
		-Wstrict-overflow=5 -Wswitch-default -Wundef -Wno-unused
		-Wimplicit-fallthrough=5 -Wswitch-enum)

	target_compile_options(jsonish
		PRIVATE
		$<$<CXX_COMPILER_ID:MSVC>:${MSVC_WARNINGS}>
		$<$<CXX_COMPILER_ID:GNU,Clang>:${GCC_WARNINGS}>)

	unset(GCC_WARNINGS)
	unset(MSVC_WARNINGS)
endif()
