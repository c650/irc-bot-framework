#ifndef _IRC_SERVER_H
#define _IRC_SERVER_H

#include <string>
#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <netinet/in.h>
#include <netdb.h>

#include "./packet.hpp"

namespace IRC {

	class Server {

		class Channel {

			std::string name;
			std::vector<std::string> modes;

			Channel(const std::string& n)
				: name(n) {}

		};

		std::string name;
		std::string address;
		unsigned int port;

		int connection_socket_fd;

		std::vector<Channel> channels;

	public:
		Server( const std::string& n, const std::string& a , const unsigned int& port );

		bool connect() const;

		void join_channel(const std::string& chan);

		std::string get_name() {
			return name;
		}
		std::string get_address() {
			return address;
		}
		unsigned int get_port() {
			return port;
		}

		const Packet& receive();
		void privmsg(const std::string& chan, const std::string& msg);

	private:

		unsigned int _send(const std::string& msg);
	};

	Server::Server( const std::string& n, const std::string& a , const unsigned int& p )
		: name(n), address(a) , port( htonl(p), connection_socket_fd(-1) )
		{}

	bool Server::connect() const {
		struct sockaddr_in serv_addr;
		struct hostent *server;

		this->connection_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (this->connection_socket_fd < 0) {
			std::cerr << "Could not get a socket fd.\n";
			return false;
		}

		server = gethostbyname(this->address.c_str());
		if (server == nullptr) {
			std::cerr << "Could not find host: " << this->address << '\n';
			close(this->connection_socket_fd);
			return false;
		}

		memset(&serv_addr, 0, sizeof(serv_addr));

		bcopy(
			(char*)server->h_addr,
			(char*)&serv_addr.sin_addr.s_addr,
			server->h_length
			);

		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = this->port;

		if ( connect(this->connection_socket_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0 ) {
			std::cerr << "Error connecting to " << this->address << '\n';
			close(this->connection_socket_fd);
			return false;
		};

		return true;
	}

	void Server::join_channel(const std::string& chan) {
		channels.push_back( Channel(chan) );

		this->_send("\rJOIN " + chan + "\r\n");
	}

	const Packet& Server::receive() {
		char *buf = new char[4096];

		read(connection_socket_fd , buf , 4095);

		Packet p(buf, this);

		delete[] buf;

		return p;
	}

	/* helpers */
	unsigned int Server::_send(const std::string& msg) {
		return write( this->connection_socket_fd , msg.data() , msg.length() );
	}

};

#endif
