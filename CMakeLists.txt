cmake_minimum_required(VERSION 3.14)
project(daw-curl-wrapper
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

add_library(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})
add_library(daw::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
add_library(daw::curl_wrapper ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC CURL::libcurl)
target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE src)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}_Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

include(CMakePackageConfigHelpers)

set_target_properties(${PROJECT_NAME} PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION 1)

set(lib${PROJECT_NAME}_INSTALL_CMAKEDIR "share/LibCurlWrapper/cmake"
        CACHE STRING "Install location for generated CMake files.")

include(CMakePackageConfigHelpers)

configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
)

write_basic_package_version_file("${PROJECT_NAME}ConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)

install(EXPORT ${PROJECT_NAME}_Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE daw::
        DESTINATION lib/cmake/${PROJECT_NAME}
        )


option(DAW_ENABLE_TESTING "Build unit tests" OFF)
if (DAW_ENABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif ()
