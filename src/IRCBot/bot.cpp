#include "./include/bot.hpp"
#include "./include/packet.hpp"
#include "./include/server.hpp"

#include "./include/default-plugins.hpp"

#include "./include/dynamic-loading.hpp"

#include <thread>
#include <mutex>

namespace IRC {

	Bot::Bot(const std::string& n, const std::string& pass, const std::vector<std::string>& _admins, const std::string& sha256_recovery_pw)
		: nick(n) , password(pass),
		  start_time(std::chrono::system_clock::now()), packets_received(0), packets_sent(0), commands_executed(0),
		  recovery_password_sha256(sha256_recovery_pw)
	{
		std::lock_guard<std::mutex> guard(this->admin_mutex); // just in case, because admins is a shared resource and should always be protected.
		for (auto& a : _admins)
			admins.push_back(a);

		/* default commands about to make your life so much easier! */
		this->add_command( (CommandInterface*)(new DefaultPlugins::Help(this, true)  ));
		this->add_command( (CommandInterface*)(new DefaultPlugins::Help(this, false) ));
		this->add_command( (CommandInterface*)(new DefaultPlugins::Loader(this)      ));
		this->add_command( (CommandInterface*)(new DefaultPlugins::Statistics(this)  ));

	}

	Bot::~Bot() {
		for (Server* s : servers) {
			if (s)
				delete s;
		}

		std::lock_guard<std::mutex> guard(this->commands_mutex);
		for (CommandInterface* c : commands) {
			if (c)
				delete c;
		}

		std::lock_guard<std::mutex> guard2( this->dynamic_plugins_mutex );
		for (auto d : dynamic_plugins) {
			if (d)
				delete d;
		}
	}

	void Bot::set_recovery_sha256(const std::string& rpw) {
		recovery_password_sha256 = rpw;
	}

	void Bot::add_server(std::string n , const std::string& a , const int& port, bool with_ssl) {
		for (const Server* s : servers) {
			if (s->get_name() == n)
				n.append("_");
		}
		servers.push_back( new Server(n , a , port, with_ssl) );
	}

	void Bot::connect_server(const std::string& server_name) {
		auto it = _find_server_by_name(server_name);
		if (it == servers.end())
			return;
		(*it)->start_connect();
		(*it)->log_on(nick, password);
	}

	bool Bot::join_channel(const std::string& server_name , const std::string& channel_name) {
		auto it = _find_server_by_name(server_name);
		if (it == servers.end())
			return false;

		(*it)->join_channel(channel_name);
		return true;
	}

	void Bot::add_a(const Bot::RELATIONSHIP& r , const std::string& user) {
		switch(r) {
		case RELATIONSHIP::ADMIN: {

			if (_is_admin(user)) {
				return;
			}

			std::lock_guard<std::mutex> guard_admin(admin_mutex);
			admins.push_back(user);
			break; // this is important!

		} case RELATIONSHIP::IGNORED: {

			if (_is_ignored(user)) {
				return;
			}

			std::lock_guard<std::mutex> guard_ignored(ignored_mutex);
			ignored.push_back(user);
			break;
		}}
	}

	bool Bot::reauthenticate(const std::string& user, const std::string hash_of_password) {
		if (hash_of_password == this->recovery_password_sha256) {
			this->add_a(Bot::RELATIONSHIP::ADMIN, user);
			return true;
		}
		return false;
	}

	void Bot::add_command( CommandInterface* cmd ) {
		if (cmd->bot() != nullptr && cmd->bot() != this) {
			std::cerr << "You tried to add a command that references a different bot.\n";
			delete cmd;
			return;
		}

		std::lock_guard<std::mutex> guard(this->commands_mutex);
		commands.push_back(cmd);
	}

	void Bot::add_dynamic_command( DynamicPluginLoading::DynamicPlugin* plugin ) {

		this->commands.push_back( plugin->get_instance(this) );

		std::lock_guard<std::mutex> guard( this->dynamic_plugins_mutex );
		this->dynamic_plugins.push_back( plugin );
	}

	void Bot::remove_dynamic_command( const std::string& name ) {

		std::lock_guard<std::mutex> guard( this->dynamic_plugins_mutex );

		for ( size_t i = 0; i < this->dynamic_plugins.size(); ++i ) {
			if (this->dynamic_plugins[i]->get_name() == name) {
				std::swap( this->dynamic_plugins[i] , this->dynamic_plugins.back());

				delete this->dynamic_plugins.back();
				this->dynamic_plugins.pop_back();

				return;
			}
		}

		throw std::runtime_error("No such plugin could be removed: " + name);
	}

	std::vector<const CommandInterface *> Bot::get_commands(void) const {
		/* not using mutex here because this will be called from a command,
			meaning that that thread owns the commands while this gets called.

			Relocking mutex causes undefined behaviour.
		*/

		std::vector<const CommandInterface *> cmds;

		for (auto c : this->commands) {
			cmds.push_back(c);
		}

		return cmds;
	}

	void Bot::listen() {
		std::vector<std::thread> threads;
		for (Server* s : servers) {
			threads.push_back(std::thread([this, s]{this->_listen_to_server(s);} ));
		}
		for (auto& t : threads)
			t.join();
	}

	void Bot::_listen_to_server(Server* s) {
		bool got_resp = true; /* checks if at least one server gives resp. */
		while(got_resp) {
			got_resp = false;

			Packet p;

			try {
				p = s->receive();
			} catch (std::exception& e) {
				std::cerr << e.what() << '\n';
				continue;
			}

			p.owner = s;

			if (p.is_valid()) {
				this->packets_received++;
				this->_check_for_triggers(p);
				got_resp = true;
			}
			got_resp = got_resp || !p.content.empty();
		}
	}

	/* helpers */

	void Bot::_check_for_triggers(const Packet& p) {

		bool sender_is_admin = _is_admin(p.sender);
		bool sender_is_ignored = _is_ignored(p.sender);

		/* These are default commands, raw functionality, internal stuff... */
		if (p.type == Packet::PacketType::NICK) {

			/* update admins and ignored based on nick changes. */

			std::unique_lock<std::mutex> guard_admin(admin_mutex); // using unique lock so that I can unlock before scope ends.
			for (auto& s : admins)
				if (s == p.sender)
					s = p.content;
			guard_admin.unlock();

			std::lock_guard<std::mutex> guard_ignored(ignored_mutex);
			for (auto& i : ignored)
				if (i == p.sender)
					i = p.content;
			// not unlocking because Scope ends anyway.

		} else if (p.type == Packet::PacketType::INVITE && sender_is_admin) {
			p.owner->join_channel( p.content );
		} else if (p.type == Packet::PacketType::KICK) {
			p.owner->join_channel( p.channel );
		}

		if (sender_is_ignored)
			return;

		std::lock_guard<std::mutex> guard(this->commands_mutex);

		/* Here we go through the user-added Commands */
		for (auto command : this->commands) {    /* checks sender's perms.... */
			if (command->triggered(p) && (sender_is_admin || !command->requires_admin())) {
				command->run(p);

				std::lock_guard<std::mutex> guard(this->stat_mutex); // stat-tracking commands requires this :)
				this->commands_executed++;
			}
		}

	}

	// TODO: Just return Server*'s for less overhead and bullsh*t
	std::vector<Server*>::iterator Bot::_find_server_by_name(const std::string& n) {

		for (size_t i = 0; i < servers.size(); ++i) {
			if (servers.at(i)->get_name() == n) {
				return servers.begin() + i;
			}
		}
		return servers.end();
	}

	bool Bot::_is_admin(const std::string& person) {
		std::lock_guard<std::mutex> guard(this->admin_mutex); // protect admins

		for (auto& a : admins)
			if (a == person)
				return true;
		std::cout << person << " is not an admin.\n";
		return false;
	}

	bool Bot::_is_ignored(const std::string& person) {
		std::lock_guard<std::mutex> guard(this->ignored_mutex); // protect ignore list.

		for (auto& a : ignored)
			if (a == person)
				return true;
		return false;
	}


	std::vector<std::string> Bot::get_stats(void) {
		std::lock_guard<std::mutex> guard(this->stat_mutex);

		time_t t = std::chrono::system_clock::to_time_t(start_time);
		return std::vector<std::string>{ "Up Since: " + std::string(std::ctime(&t)),
				 "Packets Received: " + std::to_string(this->packets_received),
				 "Packets Sent: " + std::to_string(this->packets_sent),
				 "Commands Executed: " + std::to_string(this->commands_executed)
			 };
	}
};
