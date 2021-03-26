// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file license or copy at http://www.boost.org/license_1_0.txt)
//
// Official repository: https://github.com/beached/curl_wrapper
//

#pragma once

#include <curl/curl.h>
#include <memory>
#include <string>
#include <string_view>

namespace daw::cw_details {
	class curl_headers;

	struct curl_deleter {
		void operator( )( CURL *ptr ) const;
	};
} // namespace daw::cw_details

namespace daw {
	class curl_wrapper {
		std::unique_ptr<CURL, cw_details::curl_deleter> m_curl;
		std::unique_ptr<cw_details::curl_headers> m_headers;

	public:
		curl_wrapper( );
    ~curl_wrapper( );
    curl_wrapper( curl_wrapper const & ) = delete;
    curl_wrapper & operator=( curl_wrapper const & ) = delete;
		curl_wrapper( curl_wrapper && ) = default;
    curl_wrapper & operator=( curl_wrapper && ) = default;

		explicit operator CURL *( );
		void add_header( std::string_view name, std::string_view value );
		void reset( );
		[[nodiscard]] std::string get_string( std::string_view url );
	}; // curl_wrapper
} // namespace daw
