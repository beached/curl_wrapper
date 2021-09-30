// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, version 1.0. (see accompanying
// file license or copy at http://www.boost.org/license_1_0.txt)
//
// Official repository: https://github.com/beached/curl_wrapper
//

#include "daw/curl_wrapper.h"

#include <curl/curl.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>

namespace daw::cw_details {
	inline namespace {
		[[nodiscard]] bool ensure_global_init( ) noexcept {
			static bool const m_init = []( ) {
				return curl_global_init( CURL_GLOBAL_DEFAULT );
			}( ) == 0;

			return m_init;
		}

		[[nodiscard]] auto init_curl( ) {
			if( not ensure_global_init( ) ) {
				throw std::runtime_error(
				  "Could not initialize global cURL interface" );
			}
			return curl_easy_init( );
		}

		struct curl_slist_deleter {
			inline void operator( )( curl_slist *ptr ) const {
				curl_slist_free_all( ptr );
			}
		};
	} // namespace

	class curl_headers {
		std::unique_ptr<curl_slist, curl_slist_deleter> m_values{ };

	public:
		curl_headers( ) = default;

		void add_header( std::string_view name, std::string_view value ) {
			auto header = std::string( );
			header.reserve( std::size( name ) + std::size( value ) + 1 );
			header.append( std::data( name ), std::size( name ) );
			header += ':';
			header.append( value.data( ), value.size( ) );

			if( auto p = curl_slist_append( m_values.get( ), header.data( ) );
			    not m_values ) {

				if( not p ) {
					throw std::runtime_error(
					  "curl_slist_append should always set list to non_null" );
				}
				m_values.reset( p );
			}
		}

		void reset( ) {
			m_values.reset( );
		}

		[[nodiscard]] explicit operator bool( ) const {
			return m_values != nullptr;
		}

		[[nodiscard]] curl_slist const *get( ) const {
			return m_values.get( );
		}

		[[nodiscard]] curl_slist *get( ) {
			return m_values.get( );
		}
	}; // curl_headers

	void curl_deleter::operator( )( CURL *ptr ) const {
		curl_easy_cleanup( ptr );
	}
} // namespace daw::cw_details

namespace daw {
	struct curl_wrapper::impl_t {
		cw_details::curl_headers headers{ };
		std::string body{ };
		std::unique_ptr<CURL, cw_details::curl_deleter> curl{
		  cw_details::init_curl( ) };

		inline impl_t( ) = default;

		bool has_body = false;

		void reset( ) {
			headers.reset( );
			body.clear( );
			has_body = false;
		}
	};

	curl_wrapper::curl_wrapper( )
	  : m_impl( std::make_unique<impl_t>( ) ) {}

	curl_wrapper::operator CURL *( ) {
		if( not m_impl->curl ) {
			throw std::runtime_error( "Attempt to use null cURL" );
		}
		return m_impl->curl.get( );
	}

	void curl_wrapper::add_header( std::string_view name,
	                               std::string_view value ) {
		m_impl->headers.add_header( name, value );
	}

	namespace cw_details {
		inline namespace {
			std::size_t write_handler( void *data_ptr, std::size_t Size,
			                           std::size_t nmemb, void *result ) noexcept {
				std::size_t const size = nmemb * Size;
				if( size > 0 ) {
					auto &str_result = *reinterpret_cast<std::string *>( result );
					try {
						str_result.append( reinterpret_cast<char const *>( data_ptr ),
						                   size );
					} catch( std::bad_alloc const & ) {
						// Error while appending string and with strong exception guarantee
						return 0;
					} catch( std::length_error const & ) { return 0; }
				}
				return size;
			}

			std::size_t header_handler( void *, std::size_t Size, std::size_t nmemb,
			                            void * ) noexcept {
				std::size_t const size = nmemb * Size;
				return size;
			}

		} // namespace
	}   // namespace cw_details

	std::string curl_wrapper::get_string( std::string_view url ) {
		curl_easy_setopt( m_impl->curl.get( ), CURLOPT_NOSIGNAL, 1 );
		curl_easy_setopt( m_impl->curl.get( ), CURLOPT_ACCEPT_ENCODING, "deflate" );
		curl_easy_setopt( m_impl->curl.get( ), CURLOPT_URL, url.data( ) );
		if( m_impl->headers ) {
			curl_easy_setopt( m_impl->curl.get( ), CURLOPT_HEADER, true );
			curl_easy_setopt( m_impl->curl.get( ), CURLOPT_HTTPHEADER,
			                  m_impl->headers.get( ) );
		}
		std::string result;
		curl_easy_setopt( m_impl->curl.get( ), CURLOPT_WRITEFUNCTION,
		                  cw_details::write_handler );
		curl_easy_setopt( m_impl->curl.get( ), CURLOPT_HEADERFUNCTION,
		                  cw_details::header_handler );
		if( m_impl->has_body ) {
			curl_easy_setopt( m_impl->curl.get( ), CURLOPT_POSTFIELDS,
			                  m_impl->body.c_str( ) );
		}
		curl_easy_setopt( m_impl->curl.get( ), CURLOPT_WRITEDATA, &result );

		auto const curl_result = curl_easy_perform( m_impl->curl.get( ) );
		if( curl_result != CURLE_OK ) {
			throw std::runtime_error( curl_easy_strerror( curl_result ) );
		}
		if( m_impl->headers ) {
			// Cut out response headers
			auto pos = result.find( "\r\n\r\n" );
			if( pos != std::string::npos ) {
				result = result.substr( pos + 4 );
			}
		}
		return result;
	}

	void curl_wrapper::reset( ) {
		m_impl->reset( );
		m_impl->curl.reset( cw_details::init_curl( ) );
	}

	void curl_wrapper::set_body( std::string_view value ) {
		m_impl->body.clear( );
		m_impl->body.assign( std::data( value ), std::size( value ) );
		m_impl->has_body = true;
	}

	curl_wrapper::~curl_wrapper( ) = default;
} // namespace daw
