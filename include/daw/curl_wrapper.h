// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file license or copy at http://www.boost.org/license_1_0.txt)
//
// Official repository: https://github.com/beached/curl_wrapper
//

#pragma once

#include <curl/curl.h>
#include <functional>
#include <memory>
#include <string>
#include <string_view>

namespace daw::cw_details {
	struct curl_deleter {
		void operator( )( CURL *ptr ) const;
	};
} // namespace daw::cw_details

namespace daw {
	class curl_wrapper {
		struct impl_t;
		std::unique_ptr<impl_t> m_impl;

	public:
		curl_wrapper( );
		~curl_wrapper( );
		curl_wrapper( curl_wrapper && ) = default;
		curl_wrapper &operator=( curl_wrapper && ) = default;

		curl_wrapper( curl_wrapper const & ) = delete;
		curl_wrapper &operator=( curl_wrapper const & ) = delete;

		CURL *get_curl_ptr( );
		CURL const *get_curl_ptr( ) const;

		[[nodiscard]] inline explicit operator CURL *( ) {
			return get_curl_ptr( );
		}

		[[nodiscard]] inline explicit operator CURL const *( ) const {
			return get_curl_ptr( );
		}

		[[nodiscard]] inline bool is_valid( ) const {
			return static_cast<bool>( get_curl_ptr( ) );
		}

		[[nodiscard]] inline explicit operator bool( ) const {
			return static_cast<bool>( get_curl_ptr( ) );
		}

		void add_header( std::string_view name, std::string_view value );
		void set_body( std::string_view value );

		void visit_headers(
		  std::function<void( std::string_view key, std::string_view value )> cb );

		template<typename T>
		CURLcode set_opt( CURLoption opt, T &&value ) {
			return curl_easy_setopt( get_curl_ptr( ), opt,
			                         ( std::forward<T>( value ) ) );
		}

		void reset( );

		// Get full response from server, including headers
		void retrieve( std::string_view url );

		// Get only body from server
		[[nodiscard]] std::string const &get_body( ) const;
	}; // curl_wrapper
} // namespace daw
