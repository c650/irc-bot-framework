#ifndef _SSL_CONNECTION_H
#define _SSL_CONNECTION_H

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <string>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include "./plain_connection.hpp"

namespace SSLWrapper {

	class SSLConnection : public PlainConnection {

		class _ssl_init_object {
		  public:
			_ssl_init_object() {
				SSL_load_error_strings();
				SSL_library_init();
			}
			~_ssl_init_object() {
				ERR_remove_thread_state(nullptr);
				ERR_free_strings();
				EVP_cleanup();
				CRYPTO_cleanup_all_ex_data();
				sk_SSL_COMP_free(SSL_COMP_get_compression_methods());
			}
		};

		SSL_CTX *ssl_context;
		SSL     *ssl_handle;

	public:

		SSLConnection(const std::string& host , const int& port);
		~SSLConnection();

		bool do_connect(void);
		void disconnect(void);

		std::string receive(void);
		int send(const std::string& msg);

		bool is_connected(void) {
			return PlainConnection::is_connected();
		}

		bool is_secure(void) {
			return true;
		}

		bool operator==(SSLConnection& other) const {
			return false;
		}

	};

};

#endif
