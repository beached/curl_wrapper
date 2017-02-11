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

#include <boost/utility/string_view.hpp>
#include <curl/curl.h>
#include <stdexcept>

#include <daw/daw_exception.h>

#include <string>

#include "curl_wrapper.h"

namespace daw {
	namespace impl {
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
		}	// namespace anonymous

		class curl_headers {
			curl_slist * m_values;
		public:

			curl_headers( ):
				m_values{ nullptr } { }

			~curl_headers( );
			curl_headers( curl_headers const & ) = delete;
			curl_headers( curl_headers && ) = default;
			curl_headers & operator=( curl_headers const & ) = delete;
			curl_headers & operator=( curl_headers && ) = default;

			void add_header( boost::string_view name, boost::string_view value ) {
				std::string const header = name.to_string( ) + ": " + value.to_string( );
				m_values = curl_slist_append( m_values, header.data( ) );
				daw::exception::dbg_throw_on_null( m_values, "curl_slist_append should always set list to non_null" );
			}

			explicit operator bool( ) const {
				return nullptr != m_values;
			}

			curl_slist const * get( ) const {
				return m_values;
			}

			curl_slist * get( ) {
				return m_values;
			}
		};	// curl_headers

		curl_headers::~curl_headers( ) {
			if( nullptr != m_values ) {
				curl_slist_free_all( std::exchange( m_values, nullptr ) );
			}
		}
	}	// namespace anonymous

	curl_wrapper::curl_wrapper( ): 
		m_curl{ impl::init_curl( ) },
		m_headers{ std::make_unique<impl::curl_headers>( ) } { }

	curl_wrapper::~curl_wrapper( ) {
		if( nullptr != m_curl ) {
			curl_easy_cleanup( std::exchange( m_curl, nullptr ) );
		}
	}

	curl_wrapper::operator CURL *( ) {
		daw::exception::dbg_throw_on_null( m_curl, "Attempt to use null cURL" );
		return m_curl;
	}

	void curl_wrapper::add_header( boost::string_view name, boost::string_view value ) {
		m_headers->add_header( name, value );	
	}

	namespace impl {
		namespace {
			size_t write_handler( void * data_ptr, size_t Size, size_t nmemb, void * result ) noexcept {
				size_t const size = nmemb * Size;
				if( size > 0 ) {
					auto & str_result = *static_cast<std::string *>( result );
					try {
						str_result.append( static_cast<char const *>( data_ptr ), size );
					} catch( std::bad_alloc const & ) {
						// Error while appending string and with strong exception guarantee
						return 0;
					} catch( std::length_error const & ) {
						return 0;
					}
				}
				return size;
			}

			size_t header_handler( void *, size_t Size, size_t nmemb, void * ) noexcept {
				size_t const size = nmemb * Size;
				return size;
			}

		}	// namespace anonymous
	}	// namespace impl
		
	std::string curl_wrapper::get_string( boost::string_view url ) {
		curl_easy_setopt( m_curl, CURLOPT_NOSIGNAL, 1 );
		curl_easy_setopt( m_curl, CURLOPT_ACCEPT_ENCODING, "deflate" );
		curl_easy_setopt( m_curl, CURLOPT_URL, url.data( ) );
		if( *m_headers ) {
			curl_easy_setopt( m_curl, CURLOPT_HEADER, true );
			curl_easy_setopt( m_curl, CURLOPT_HTTPHEADER, m_headers->get( ) );
		}
		std::string result;
		curl_easy_setopt( m_curl, CURLOPT_WRITEFUNCTION, impl::write_handler );
		curl_easy_setopt( m_curl, CURLOPT_HEADERFUNCTION, impl::header_handler );
		curl_easy_setopt( m_curl, CURLOPT_WRITEDATA, &result );
		
		auto const curl_result = curl_easy_perform( m_curl );
		daw::exception::daw_throw_on_false<std::runtime_error>( CURLE_OK == curl_result, curl_easy_strerror( curl_result ) );

		return result;
	}

}	// namespace daw

