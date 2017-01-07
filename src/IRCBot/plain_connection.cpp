#include "./include/plain_connection.hpp"

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <iostream>
#include <string>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

namespace SSLWrapper {

	PlainConnection::PlainConnection(const std::string& _address, const int& _port)
		: address(_address), port(_port), connection_socket_fd(-1)
		{
			std::cout << "PlainConnection(...)\n";
		}

	PlainConnection::~PlainConnection() {
		this->disconnect();
	}

	bool PlainConnection::do_connect(void) {
		struct sockaddr_in serv_addr;
		struct hostent *server;

		this->connection_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (this->connection_socket_fd < 0) {
			std::cerr << "Could not get a socket fd.\n";
			return false;
		}

		server = gethostbyname(this->address.data());
		if (server == nullptr) {
			std::cerr << "Could not find host: " << this->address << '\n';
			close(this->connection_socket_fd);
			return false;
		}

		memset(&serv_addr, 0, sizeof(serv_addr));

		memcpy(	(char*)&serv_addr.sin_addr.s_addr, (char*)server->h_addr, server->h_length );

		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(this->port);

		if ( connect(this->connection_socket_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0 ) {
			std::cerr << "Error connecting to " << this->address << '\n';
			close(this->connection_socket_fd);
			return false;
		};

		std::cout << "Connection Success.\n";
		return true;
	}

	void PlainConnection::disconnect(void) {
		if (this->connection_socket_fd < 0)
			return;
		close(this->connection_socket_fd);
		this->connection_socket_fd = -1;
	}

	std::string PlainConnection::receive(void) {
		std::string s = "";
		if ( !this->is_connected() )
			return s;

		char *buf = new char[4096];
		for (size_t i = 0; i < 4096; ++i)
			buf[i] = 0x00;
		if (read(this->connection_socket_fd, buf, 4095) > 0) {
			s = buf;
			std::cout << s << '\n';
		}

		delete [] buf;
		return s;
	}

	int PlainConnection::send(const std::string& msg) {
		if ( !this->is_connected() )
			return -1;

		int ret;
		for (size_t i = 0; i < msg.length(); ) {
			ret = write(this->connection_socket_fd, msg.data()+i, msg.length()-i);
			if (ret > 0) {
				i += ret;
			} else {
				std::cerr << "PlainConnection::send(): Could not send data\n";
				return ret;
			}
		}

		// int ret = write(this->connection_socket_fd , msg.data(), msg.length());
		// if (ret < 0) {
		// 	std::cerr << "Could not send data.\n";
		// 	this->disconnect();
		// }
		return ret;
	}
};
