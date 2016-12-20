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

		std::vector<Server> servers;

		std::unordered_map<std::string, std::function<void(const Packet& p)>> actions;

	public:

		Bot();

		void add_server(const std::string& n , const std::string& a , const unsigned int& port);

		bool join_channel(const std::string& server_name , const std::string& channel_name);
		bool join_channel(const std::vector<Server>::iterator& it, const std::string& channel_name);

		template <class Func>
		void add_func(const std::string& trigger, Func f);

		void listen();

	};

};

#endif
