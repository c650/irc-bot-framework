#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <string>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

#include <iostream>

#include "./packet.hpp"

namespace IRC {

	Server::Server( const std::string& n, const std::string& a , const unsigned int& p )
		: name(n), address(a) , port( p ), connection_socket_fd(-1)
		{}

	Server::~Server() {
		_send("\rQUIT :Quit: Leaving...\r\n");
		close(connection_socket_fd);
	}


	bool Server::start_connect() {

		std::cout << "Attempting to connect server: " << name << " at " << address << '\n';

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

	void Server::join_channel(const std::string& chan) {
		bool found_chan = false;
		for (size_t i = 0; i < channels.size() ; ++i)
			found_chan = channels.at(i).name == chan;

		if (!found_chan)
			channels.push_back( Channel(chan) );

		this->_send("\rJOIN " + chan + "\r\n");
	}

	Packet Server::receive() const {
		char *buf = new char[4096];
		for (int i = 0; i < 4096; ++i)
			buf[i] = 0x00;

		int n = read(connection_socket_fd , buf , 4095);
		std::cout << "Read " << n << " bytes\n";

		std::string s = buf;
		if (s.substr(0,4) == "PING") {
			s.replace(0,2, "\rPO");
			_send(s);
		}

		// add auto-rejoin here.
		// add invite here...

		Packet p(buf, this);

		delete[] buf;

		return p;
	}

	void Server::privmsg(const std::string& chan, const std::string& msg) const {
		this->_send("\rPRIVMSG " + chan + " :" + msg + "\r\n");
	}

	/* helpers */
	unsigned int Server::_send(const std::string& msg) const {
		std::cout << "Sending: " << msg;
		return write( this->connection_socket_fd , msg.data() , msg.length() );
	}

	void Server::log_on(const std::string& nick, const std::string& pass) const {
		this->_send("\rUSER " + nick + " 0 * :" + nick + "\r\n");
		this->_send("\rNICK " + nick + "\r\n");

		if (!pass.empty()) {
			this->_send("\rPRIVMSG NickServ :identify " + pass + "\r\n");
		}
	}


};
