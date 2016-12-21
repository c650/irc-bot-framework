#ifndef _IRC_BOT_H
#define _IRC_BOT_H

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

#include <algorithm>

#include "./types.hpp"
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
		std::vector<Server>::iterator _find_server_by_name(const std::string& n);
	};

	template <class Func>
	void Bot::on_privmsg(const std::string& trigger, Func f) {
		actions[trigger] = f;
	}
};
#endif
