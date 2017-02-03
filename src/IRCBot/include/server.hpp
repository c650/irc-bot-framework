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

#include <iostream>

namespace IRC {

	class Server {

		std::string name;


		std::string nick,
		            pass;

		std::vector< std::string > channels;

		SSLWrapper::BaseConnection * const connection;


	public:

		/*
			constructor

			@param n the name of the server.
			@param a the address (e.g., irc.example.net)
			@param port the port to connect to.
			@param with_ssl whether to use SSL (TLSv1)
		*/
		Server( const std::string& n, const std::string& a , const int& port, bool with_ssl = true );

		/*
			Sends a default quit message and then disconnects.
		*/
		~Server();

		/*
			Starts a connection.
			Calls its `connection`'s do_connect(void)

			@return the success of the connection.
		*/
		bool start_connect();

		/*
			Performs the login commands to the server. Saves nick and pass
			for any later use.

			@param nick, pass the nick and pass to use to log on. If pass == "",
			the bot will not attempt to contact NickServ.
		*/
		void log_on(const std::string& nick, const std::string& pass);

		/*
			Sends a quit message and then closes the connection
			5 seconds later.

			@param msg the quit message, defaulting to "Leaving"
		*/
		void disconnect(const std::string& msg = "Leaving");

		/*
			join/part a channel

			@param chan the channel to join/part
		*/
		void join_channel(const std::string& chan);
		void part_channel(const std::string& chan);

		std::string get_name() const {
			return name;
		}
		std::string get_nick(void) const {
			return nick;
		}

		void set_nick(const std::string& new_nick);

		/*
			Read data from the server and parse into packet.

			@return a Packet from the server.
		*/
		Packet receive();

		/*
			Send a privmsg on the server

			@param chan the channel to send the message to.
			@param msg the message to send.
		*/
		void privmsg(const std::string& chan, const std::string& msg) const;

		/*
			Sends a KICK to the server to try to kick the user.

			@param chan the channel
			@param user the user.
		*/
		void kick(const std::string& chan, const std::string& user) const;

		bool operator==(Server& other) const {
			return (this == &other) || (this->name == other.name && this->connection->socket_fd() == other.connection->socket_fd());
		}

		void print_connection(void) const {
			std::cout << "connection = " << this->connection << '\n';
		}

	private:

		/*
			Local, private helper that handles writing to the socket
			by calling BaseConnection's send().

			@param msg the message to send
			@return the number of characters written.
		*/
		unsigned int _send(const std::string& msg) const;
	};
};

#endif
