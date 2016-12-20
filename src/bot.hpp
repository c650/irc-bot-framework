#ifndef _IRC_BOT_H
#define _IRC_BOT_H

#include <string>
#include <vector>
#include <unordered_map>

#include "./server.hpp"
#include "./packet.hpp"

namespace IRC {

	class Bot {

		std::string nick;
		std::string password;

		std::vector<std::string> admins,
		                         ignored;

		std::vector<Server> servers;

		std::unordered_map<std::string, std::function<void(const Packet& p)>> actions;

	public:

		Bot(const std::string& n, const std::string& pass, const std::string& first_admin);

		void add_server(const std::string& n , const std::string& a , const unsigned int& port);
		void connect_server(const std::string& server_name);
		void connect_server(const std::vector<Server>::iterator& it);

		bool join_channel(const std::string& server_name , const std::string& channel_name);
		bool join_channel(const std::vector<Server>::iterator& it, const std::string& channel_name);

		/*
			on some trigger, use function f.

			f has to be:
				void f(const Packet& p);
		*/
		template <class Func>
		void on_privmsg(const std::string& trigger, Func f);

		template <class Func>
		void on_join(Func f);

		template <class Func>
		void on_kick(Func f);

		void on_nick();

		void listen();

	private:

		void _check_for_triggers(const Packet& p);

	};

	Bot::Bot(const std::string& n, const std::string& pass, const std::string& first_admin)
		: nick(n) , password(pass)
	{
		admins.push_back(first_admin);
	}

	void Bot::add_server(const std::string& n , const std::string& a , const unsigned int& port) {
		servers.push_back( Server(n,a,port) );
	}

	void Bot::connect_server(const std::string& server_name) {
		this->connect_server( std::find(servers.begin(), servers.end(), [&](const Server& s){return s.get_name() == server_name;}) );
	}

	void Bot::connect_server(const std::vector<Server>::iterator& it) {
		if (it == servers.end())
			return;
		it->connect();
	}

	bool Bot::join_channel(const std::string& server_name , const std::string& channel_name) {
		return this->join_channel( std::find(servers.begin(), servers.end(), [&](const Server& s){return s.get_name() == server_name;}) , channel_name );
	}
	bool Bot::join_channel(const std::vector<Server>::iterator& it, const std::string& channel_name) {
		if (it == servers.end())
			return false;

		it->join_channel(channel_name);
		return true;
	}

	template <class Func>
	void Bot::on_privmsg(const std::string& trigger, Func f) {
		actions[trigger] = f;
	}

	void Bot::listen() {
		bool got_resp = false; /* checks if at least one server gives resp. */
		do {
			got_resp = false;

			Packet p;
			for (auto& s : servers) {
				p = s.receive();
				if (p.is_valid()) {
					this->_check_for_triggers(p);
					got_resp = true;
				}
			}

		} while (got_resp);
	}

	/* helpers */

	void Bot::_check_for_triggers(const Packet& p) {
		/*if (p.type == "JOIN") {

		} else if (p.type == "KICK") {

		} else*/ if (p.type = "PRIVMSG") {
			for (auto it = actions.begin() ; it != actions.end() ; ++it) {
				if (p.content.substr(0 , it->first.length()) == it->first) {
					it->second(p);
				}
			}
		}
	}


};

#endif
