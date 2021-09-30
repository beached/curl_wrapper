cmake_minimum_required(VERSION 3.14)
project(curl_wrapper
        VERSION "0.1.0"
        DESCRIPTION "Wraps Curl for C++"
        HOMEPAGE_URL "https://github.com/beached/curl_wrapper"
        LANGUAGES C CXX
        )


find_package(CURL REQUIRED)
find_package(Threads REQUIRED)

set(HEADER_FILES
        include/daw/curl_wrapper.h
        )

set(SOURCE_FILES
        src/curl_wrapper.cpp
        )

include(GNUInstallDirs)

option(BUILD_SHARED_LIBS "Build shared library" ON)

add_library(curl_wrapper ${HEADER_FILES} ${SOURCE_FILES})
add_library(daw::curl_wrapper ALIAS curl_wrapper)

target_link_libraries(curl_wrapper PUBLIC CURL::libcurl)
target_include_directories( curl_wrapper PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE src)
target_compile_features(curl_wrapper PUBLIC cxx_std_17)

install(TARGETS curl_wrapper
        EXPORT curl_wrapper_Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

include(CMakePackageConfigHelpers)

set_target_properties(curl_wrapper PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION 1)

set(libcurl_wrapper_INSTALL_CMAKEDIR "share/LibCurlWrapper/cmake"
        CACHE STRING "Install location for generated CMake files.")

option(DAW_ENABLE_TESTING "Build unit tests" OFF)
if (DAW_ENABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif ()
