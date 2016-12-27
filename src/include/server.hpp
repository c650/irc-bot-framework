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

#include "./ssl_connection.hpp"

namespace IRC {

	class Server {

		std::string name;

		SSLWrapper::SSLConnection *connection;

		std::string nick,
		            pass;

		std::vector< std::string > channels;

	public:

		Server( const std::string& n, const std::string& a , const int& port, bool with_ssl = false );

		~Server();

		bool start_connect();

		void log_on(const std::string& nick, const std::string& pass);

		void disconnect(const std::string& msg = "Leaving");

		void join_channel(const std::string& chan);
		void part_channel(const std::string& chan);

		std::string get_name() const {
			return name;
		}
		std::string get_nick(void) const {
			return nick;
		}

		void set_nick(const std::string& new_nick);

		Packet receive();
		void privmsg(const std::string& chan, const std::string& msg) const;
		void kick(const std::string& chan, const std::string& user) const;

		bool operator==(Server& other) const {
			return (this == &other) || (this->name == other.name && this->connection->socket_fd() == other.connection->socket_fd());
		}

	private:

		unsigned int _send(const std::string& msg) const;
	};
};

#endif
