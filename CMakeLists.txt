cmake_minimum_required(VERSION 3.16)
project(parallel_topk VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(TOPK_BUILD_LIBRARY "Build topk library target" ON)
option(TOPK_BUILD_APPS "Build app targets" ON)
option(TOPK_BUILD_TESTS "Build test targets" ON)
option(TOPK_ENABLE_OPENMP "Enable OpenMP when available" ON)

if(PROJECT_IS_TOP_LEVEL)
	include(CTest)
endif()

file(GLOB TOPK_LIBRARY_SOURCES CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

if(TOPK_BUILD_LIBRARY AND TOPK_LIBRARY_SOURCES)
	add_library(topk_lib ${TOPK_LIBRARY_SOURCES})
	add_library(parallel_topk::topk_lib ALIAS topk_lib)

	target_include_directories(topk_lib
		PUBLIC
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
			$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	)

	if(TOPK_ENABLE_OPENMP)
		find_package(OpenMP QUIET)
		if(OpenMP_CXX_FOUND)
			target_link_libraries(topk_lib PUBLIC OpenMP::OpenMP_CXX)
			target_compile_definitions(topk_lib PUBLIC TOPK_USE_OPENMP=1)
		endif()
	endif()

	install(TARGETS topk_lib
		EXPORT parallel_topkTargets
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)

	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	)

	install(EXPORT parallel_topkTargets
		FILE parallel_topkTargets.cmake
		NAMESPACE parallel_topk::
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/parallel_topk
	)
endif()

function(topk_add_optional_executable target_name source_path)
	if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${source_path}")
		add_executable(${target_name} "${source_path}")
		if(TARGET topk_lib)
			target_link_libraries(${target_name} PRIVATE topk_lib)
		endif()
	endif()
endfunction()

if(TOPK_BUILD_APPS)
	topk_add_optional_executable(bench apps/bench_main.cpp)
	topk_add_optional_executable(debug_main apps/debug_main.cpp)
endif()

if(TOPK_BUILD_TESTS)
	topk_add_optional_executable(test_correctness tests/test_correctness.cpp)
	topk_add_optional_executable(test_edge_cases tests/test_edge_cases.cpp)

	if(TARGET test_correctness)
		add_test(NAME test_correctness COMMAND test_correctness)
	endif()
	if(TARGET test_edge_cases)
		add_test(NAME test_edge_cases COMMAND test_edge_cases)
	endif()
endif()
