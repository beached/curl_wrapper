// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file license or copy at http://www.boost.org/license_1_0.txt)
//
// Official repository: https://github.com/beached/curl_wrapper
//

#include <daw/curl_wrapper.h>

#include <iostream>

int main( ) {
	auto crl = daw::curl_wrapper( );
	std::cout << crl.get_string( "https://www.google.ca/" );
}
