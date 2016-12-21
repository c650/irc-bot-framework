#include "./bot.hpp"
#include "./packet.hpp"
#include "./server.hpp"

namespace IRC {

	Bot::Bot(const std::string& n, const std::string& pass, const std::string& first_admin)
		: nick(n) , password(pass)
	{
		admins.push_back(first_admin);
	}

	Bot::~Bot() {
		for (Server* s : servers) {
			if (s)
				delete s;
		}

		for (Command* c : commands) {
			if (c)
				delete c;
		}
	}

	void Bot::add_server(const std::string& n , const std::string& a , const unsigned int& port) {
		servers.push_back( new Server(n,a,port) );
	}

	void Bot::connect_server(const std::string& server_name) {
		this->connect_server( _find_server_by_name(server_name) );
	}

	void Bot::connect_server(const std::vector<Server*>::iterator& it) {
		if (it == servers.end())
			return;
		(*it)->start_connect();
		(*it)->log_on(nick, password);
	}

	bool Bot::join_channel(const std::string& server_name , const std::string& channel_name) {
		return this->join_channel( _find_server_by_name(server_name) , channel_name );
	}
	bool Bot::join_channel(const std::vector<Server*>::iterator& it, const std::string& channel_name) {
		if (it == servers.end())
			return false;

		(*it)->join_channel(channel_name);
		return true;
	}

	void Bot::listen() {
		bool got_resp = false; /* checks if at least one server gives resp. */
		do {
			got_resp = false;

			Packet p;
			for (Server* s : servers) {
				p = s->receive();
				p.owner = s;
				if (p.is_valid()) {
					this->_check_for_triggers(p);
					got_resp = true;
				}
				got_resp = got_resp || !p.content.empty();
			}

		} while (got_resp);
	}

	/* helpers */

	void Bot::_check_for_triggers(const Packet& p) {
		if (p.type == "PRIVMSG") {
			bool sender_is_admin = _is_admin(p.sender);
			if (p.content.substr(0, 5) == "@help") {
				for (Command* command : this->commands) {
					if (sender_is_admin || !command->requires_admin)
						p.owner->privmsg(p.sender, command->trigger + ": " + command->desc);
				}
				return;
			}
			for (Command* command : this->commands) {                                      /* checks sender's perms.... */
				if (p.content.substr(0 , command->trigger.length()) == command->trigger && ( !command->requires_admin || sender_is_admin )) {
					command->func(p);
					break;
				}
			}
		}
	}

	std::vector<Server*>::iterator Bot::_find_server_by_name(const std::string& n) {
		 for (size_t i = 0; i < servers.size(); ++i) {
			 if (servers.at(i)->get_name() == n) {
				 return servers.begin() + i;
			 }
		 }
		 return servers.end();
	}

	bool Bot::_is_admin(const std::string& person) {
		for (auto& a : admins)
			if (a == person)
				return true;
		std::cout << person << " is not an admin.\n";
		return false;
	}
};
