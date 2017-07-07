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

#pragma once

#include <curl/curl.h>
#include <memory>
#include <unordered_map>

#include <daw/daw_string_view.h>

namespace daw {
	namespace impl {
		class curl_headers;
	}

	class curl_wrapper {
		CURL * m_curl;
		std::unique_ptr<impl::curl_headers> m_headers;
	public:
		curl_wrapper( );
		~curl_wrapper( );
		curl_wrapper( curl_wrapper const & ) = delete;
		curl_wrapper( curl_wrapper && ) = default;
		curl_wrapper & operator=( curl_wrapper const & ) = delete;
		curl_wrapper & operator=( curl_wrapper && ) = default;

		operator CURL *( );

		void add_header( daw::string_view name, daw::string_view value );
		
		std::string get_string( daw::string_view url );
	};	// curl_wrapper
}    // namespace daw


