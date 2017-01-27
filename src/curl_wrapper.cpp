// The MIT License (MIT)
//
// Copyright (c) 2017 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <curl/curl.h>

#include <daw/daw_exception.h>

#include "curl_wrapper.h"

namespace daw {
	namespace {
		bool ensure_global_init( ) noexcept {
			static bool const m_init = []( ) {
				return curl_global_init( CURL_GLOBAL_DEFAULT );
			}( ) == 0;

			return m_init;
		}

		auto init_curl( ) {
			daw::exception::daw_throw_on_false( ensure_global_init( ), "Could not initialize global cURL interface" );
			return curl_easy_init( );
		}
	}

	curl_wrapper::curl_wrapper( ): 
		m_curl{ init_curl( ) } { }

	curl_wrapper::~curl_wrapper( ) {
		if( m_curl ) {
			auto tmp = m_curl;
			m_curl = nullptr;
			curl_easy_cleanup( tmp );
		}
	}

	curl_wrapper::operator CURL *( ) {
		daw::exception::dbg_throw_on_null( m_curl, "Attempt to use null cURL" );
		return m_curl;
	}
	
}	// namespace daw

