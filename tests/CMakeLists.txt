
add_executable(curl_wrapper_test src/curl_wrapper_test.cpp)
target_link_libraries(curl_wrapper_test daw::curl_wrapper )
add_test(NAME curl_wrapper_test COMMAND curl_wrapper_test)

