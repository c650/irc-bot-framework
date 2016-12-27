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

#include "./include/packet.hpp"
#include "./include/ssl_connection.hpp"

namespace IRC {

	Server::Server( const std::string& n, const std::string& a , const int& p )
		: name(n), connection(new SSLWrapper::SSLConnection(a , p) )
		{}

	Server::~Server() {
		this->disconnect();
		delete connection;
	}

	bool Server::start_connect() {
		std::cout << "Attempting to connect server: " << name << " at " << connection->get_address() << '\n';
		return connection->do_connect();
	}

	void Server::disconnect(const std::string& msg) {
		_send("\rQUIT :" + msg + "\r\n");
		sleep(5);
		connection->disconnect();
	}

	void Server::join_channel(const std::string& chan) {
		bool found_chan = false;
		for (size_t i = 0; i < channels.size() ; ++i)
			found_chan = found_chan || channels.at(i) == chan;

		if (!found_chan)
			channels.push_back( chan );

		this->_send("\rJOIN " + chan + "\r\n");
	}

	void Server::part_channel(const std::string& chan) {
		std::string chan_name = chan.substr(0, chan.find(" "));
		for (size_t i = 0 ; i < channels.size(); ++i) {
			if (channels.at(i) == chan_name) {
				channels.erase(channels.begin()+i);
				break;
			}
		}

		_send("\rPART " + chan + "\r\n");
	}

	Packet Server::receive() {

		std::string s = this->connection->receive();
		if (s.substr(0,4) == "PING") {
			s.replace(0,2, "\rPO");
			_send(s);
		}
		Packet p(s);

		return p;
	}

	void Server::privmsg(const std::string& chan, const std::string& msg) const {
		this->_send("\rPRIVMSG " + chan + " :" + msg + "\r\n");
	}

	void Server::kick(const std::string& chan, const std::string& user) const {
		this->_send("\rKICK " + chan + " " + user + "\r\n");
	}

	/* helpers */
	unsigned int Server::_send(const std::string& msg) const {
		std::cout << "Sending: " << msg;
		return this->connection->send(msg);
	}

	void Server::log_on(const std::string& n, const std::string& p) {

		this->nick = n;
		this->_send("\rUSER " + this->nick + " 0 * :" + this->nick + "\r\n");
		this->_send("\rNICK " + this->nick + "\r\n");

		this->pass = p;
		if (!this->pass.empty()) {
			this->_send("\rPRIVMSG NickServ :identify " + this->pass + "\r\n");
		}
	}

	void Server::set_nick(const std::string& new_nick) {
		this->nick = new_nick;
		this->_send("\rNICK " + this->nick + "\r\n");
	}

};
