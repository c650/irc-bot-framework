#ifndef _IRC_SERVER_H
#define _IRC_SERVER_H

#include <string>
#include <vector>

#include <cstdio>
#include <cstdlib>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

#include <iostream>

#include "./types.hpp"
#include "./server.hpp"
#include "./packet.hpp"

namespace IRC {

	class Server {

		std::string name;
		std::string address;
		int port;

		int connection_socket_fd;

		std::vector< std::string > channels;

	public:

		Server( const std::string& n, const std::string& a , const unsigned int& port );

		~Server();

		bool start_connect();
		void disconnect(const std::string& msg = "Leaving");

		void join_channel(const std::string& chan);
		void part_channel(const std::string& chan);

		std::string get_name() const {
			return name;
		}
		std::string get_address() const {
			return address;
		}
		unsigned int get_port() const {
			return port;
		}

		Packet receive();
		void privmsg(const std::string& chan, const std::string& msg) const;

		bool operator==(Server& other) const {
			return this->name == other.name && this->connection_socket_fd == other.connection_socket_fd
			       && this->port == other.port && this->address == other.address;
		}

		void log_on(const std::string& nick, const std::string& pass) const;

	private:

		unsigned int _send(const std::string& msg) const;
	};
};

#endif
