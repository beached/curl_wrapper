cmake_minimum_required( VERSION 2.8.12 )

project( curl_wrapper_prj )

include( ExternalProject )

find_package( CURL REQUIRED )

enable_testing( )

find_package( Threads REQUIRED )

if( ${CMAKE_CXX_COMPILER_ID} STREQUAL 'MSVC' )
	add_compile_options( -D_WIN32_WINNT=0x0601 /std:c++latest ) 
else( )
	if( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang" )
		add_compile_options(-std=c++14 -march=native -pthread -Weverything -Wno-c++98-compat -Wno-covered-switch-default -Wno-padded -Wno-exit-time-destructors -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-missing-noreturn -Wno-missing-prototypes -Wno-disabled-macro-expansion)
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
	elseif( ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" )
		add_compile_options(-std=c++14 -march=native -pthread -Wall -Wno-deprecated-declarations)
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
	endif( )
endif( )

externalproject_add(
    header_libraries_prj
    GIT_REPOSITORY "https://github.com/beached/header_libraries.git"
    SOURCE_DIR "${CMAKE_BINARY_DIR}/dependencies/header_libraries"
    GIT_TAG "master"
    INSTALL_DIR "${CMAKE_BINARY_DIR}/install"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
)

set( HEADER_FOLDER "include" )
set( SOURCE_FOLDER "src" )
set( TEST_FOLDER "tests" )

include_directories( SYSTEM "${CMAKE_BINARY_DIR}/install/include" )
include_directories( ${HEADER_FOLDER} )

include_directories( SYSTEM ${CURL_INCLUDE_DIRS} )

link_directories( "${CMAKE_BINARY_DIR}/install/lib" )

set( HEADER_FILES
	${HEADER_FOLDER}/curl_wrapper.h
)

set( SOURCE_FILES
	${SOURCE_FOLDER}/curl_wrapper.cpp
)

add_library( curl_wrapper ${HEADER_FILES} ${SOURCE_FILES} )
target_link_libraries( curl_wrapper ${CURL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} )
add_dependencies( curl_wrapper header_libraries_prj )

add_executable( curl_wrapper_test_bin ${HEADER_FILES} ${TEST_FOLDER}/curl_wrapper_test.cpp )
target_link_libraries( curl_wrapper_test_bin curl_wrapper ${CURL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} )

install( TARGETS curl_wrapper DESTINATION lib )
install( DIRECTORY ${HEADER_FOLDER}/ DESTINATION include/daw )

