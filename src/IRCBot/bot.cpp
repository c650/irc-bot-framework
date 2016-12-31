#include "./include/bot.hpp"
#include "./include/packet.hpp"
#include "./include/server.hpp"

namespace IRC {

	Bot::Bot(const std::string& n, const std::string& pass, const std::vector<std::string>& _admins)
		: nick(n) , password(pass)
	{
		for (auto& a : _admins)
			admins.push_back(a);
	}

	Bot::~Bot() {
		for (Server* s : servers) {
			if (s)
				delete s;
		}

		for (CommandInterface* c : commands) {
			if (c)
				delete c;
		}
	}

	void Bot::add_server(std::string n , const std::string& a , const int& port, bool with_ssl) {
		for (const Server* s : servers) {
			if (s->get_name() == n)
				n.append("_");
		}
		servers.push_back( new Server(n , a , port, with_ssl) );
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

	void Bot::add_a(const Bot::RELATIONSHIP& r , const std::string& user) {
		switch(r) {
		case RELATIONSHIP::ADMIN:
			admins.push_back(user);
		case RELATIONSHIP::IGNORED:
			ignored.push_back(user);
		}
	}

	void Bot::add_command( CommandInterface* cmd ) {
		commands.push_back(cmd);
	}

	void Bot::listen() {
		bool got_resp = false; /* checks if at least one server gives resp. */
		do {
			got_resp = false;

			Packet p;
			for (Server* s : servers) {

				try {
					p = s->receive();
				} catch (std::exception& e) {
					std::cerr << e.what() << '\n';
					continue;
				}

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

		bool sender_is_admin = _is_admin(p.sender);

		/* These are default commands, raw functionality, internal stuff... */
		if (p.type == Packet::PacketType::NICK) {

			// update admins and ignored based on nick changes.
			for (auto& s : admins)
				if (s == p.sender)
					s = p.content;

			for (auto& i : ignored)
				if (i == p.sender)
					i = p.content;

		} else if (p.type == Packet::PacketType::PRIVMSG) {
			if (p.content.substr(0, 5) == "@help") {
				for (const auto command : this->commands) {
					if (sender_is_admin || !command->requires_admin())
						p.owner->privmsg(p.sender, command->trigger() + ": " + command->desc());
				}
				return;
			}
		} else if (p.type == Packet::PacketType::INVITE && sender_is_admin) {
			p.owner->join_channel( p.content );
		} else if (p.type == Packet::PacketType::KICK) {
			p.owner->join_channel( p.channel );
		}

		/* Here we go through the user-added Commands */
		for (auto command : this->commands) {    /* checks sender's perms.... */
			if (command->triggered(p) && (sender_is_admin || !command->requires_admin())) {
				command->run(p);
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
