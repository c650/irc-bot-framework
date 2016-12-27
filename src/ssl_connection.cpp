#include "./include/plain_connection.hpp"
#include "./include/ssl_connection.hpp"

#include <iostream>

#include <cstring>
#include <string>

#include <openssl/ssl.h>

namespace SSLWrapper {

	SSLConnection::SSLConnection(const std::string& _address, const int& _port)
		: PlainConnection(_address,_port), ssl_context(nullptr), ssl_handle(nullptr)
		{
			std::cout << "SSLConnection(...)\n";
		}

	SSLConnection::~SSLConnection() {
		this->disconnect();
	}

	bool SSLConnection::do_connect(void) {
		if ( !PlainConnection::do_connect() )
			return false;

		static _ssl_init_object _ssl_init; // https://fizz.buzz/posts/openssl-sockets-in-c++-part-4.html

		ssl_context = SSL_CTX_new(TLSv1_client_method());
		if (ssl_context == nullptr) {
			std::cerr << "Couldn't do SSL context\n";
			return false;
		}
		SSL_CTX_set_mode(ssl_context, SSL_MODE_AUTO_RETRY);

		ssl_handle = SSL_new(ssl_context);
		if (ssl_handle == nullptr) {
			SSL_CTX_free(ssl_context);
			ssl_context = nullptr;
			std::cerr << "Couldn't do new SSL\n";
			return false;
		}

		if( !SSL_set_fd(ssl_handle, this->connection_socket_fd) ) {
			SSL_free(ssl_handle);
			SSL_CTX_free(ssl_context);
			ssl_handle = nullptr;
			ssl_context = nullptr;
			std::cerr << "failed to set FD\n";
			return false;
		}

		if (SSL_connect(ssl_handle) != 1) {
			std::cerr << "connection failed\n";
			return false;
		}

		return true;
	}

	void SSLConnection::disconnect(void) {
		if (ssl_handle) {
			SSL_shutdown(ssl_handle);
			SSL_free(ssl_handle);
			ssl_handle = nullptr;
		}

		if (ssl_context) {
			SSL_CTX_free(ssl_context);
			ssl_context = nullptr;
		}

		PlainConnection::disconnect();
	}

	std::string SSLConnection::receive(void) {
		std::string s = "";
		if (!this->is_connected())
			return s;

		char *buf = new char[4096];
		for (size_t i = 0; i < 4096; ++i)
			buf[i] = 0x00;

		if ( SSL_read( this->ssl_handle , buf, 4095) > 0) {
			s = buf;
		}

		delete [] buf;
		return s;
	}

	int SSLConnection::send(const std::string& msg) {
		if (!this->is_connected())
			return -1;

		// int ret;
		// for (size_t i = 0; i < msg.length(); /* see 94 */) {
		// 	ret = SSL_write(ssl_handle, msg.data()+i, msg.length()-i);
		// 	if (ret > 0) {
		// 		i += ret;
		// 	} else {
		// 		switch(SSL_get_error(ssl_handle, sent))
		// 	}
		// }
		int ret = SSL_write(ssl_handle, msg.data(), msg.length());
		if (ret < 0) {
			std::cerr << "Failed to send data. Disconnecting...\n";
			this->disconnect();
		}

		return ret;
	}



};
