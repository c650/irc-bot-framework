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

		struct Command {
			std::string trigger, desc;
			std::function<void(const Packet&)> func;
			bool requires_admin;

			Command(const std::string& t , std::function<void(const Packet&)> f, const std::string& d, bool ra)
				: trigger(t), func(f), desc(d), requires_admin(ra) {}
		};

		std::string nick;
		std::string password;

		std::vector<std::string> admins,
								 ignored;

		std::vector< Server* > servers;

		std::vector<Command*> commands;

	public:

		enum class RELATIONSHIP {
			ADMIN,
			IGNORED
		};

		Bot(const std::string& n, const std::string& pass, const std::string& first_admin);
		~Bot();

		void add_server(std::string n , const std::string& a , const int& port, bool with_ssl = true);
		void connect_server(const std::string& server_name);
		void connect_server(const std::vector<Server*>::iterator& it);

		void disconnect_server(Server *s);

		bool join_channel(const std::string& server_name , const std::string& channel_name);
		bool join_channel(const std::vector<Server*>::iterator& it, const std::string& channel_name);

		void add_a(const RELATIONSHIP& r , const std::string& user);

		/*
			on some trigger, use function f.

			f has to be:
				void f(const Packet& p);
		*/
		template <class Func>
		void on_privmsg(const std::string& trigger, Func f, const std::string& desc, bool requires_admin = false);

		template <class Func>
		void on_join(Func f);

		template <class Func>
		void on_kick(Func f);

		void on_nick();

		void listen();

	private:

		void _check_for_triggers(const Packet& p);
		std::vector<Server*>::iterator _find_server_by_name(const std::string& n);

		bool _is_admin(const std::string& person);
	};

	template <class Func>
	void Bot::on_privmsg(const std::string& trigger, Func f, const std::string& desc, bool requires_admin) {
		commands.push_back( new Command(trigger, f , desc, requires_admin) );
	}
};
#endif
