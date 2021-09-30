set(CMAKE_CXX_STANDARD 17 CACHE STRING "The C++ standard whose features are requested.")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(curl_wrapper_test src/curl_wrapper_test.cpp)
target_link_libraries(curl_wrapper_test daw::daw-curl-wrapper )
add_test(NAME curl_wrapper_test COMMAND curl_wrapper_test)

