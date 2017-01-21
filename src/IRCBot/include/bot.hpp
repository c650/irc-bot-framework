#ifndef _IRC_BOT_H
#define _IRC_BOT_H

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

#include <chrono>
#include <ctime>

#include <algorithm>
#include <mutex>

#include "./types.hpp"
#include "./command-interface.hpp"
#include "./server.hpp"
#include "./packet.hpp"
#include "./dynamic-loading.hpp"

namespace IRC {

	class Bot {

		std::string nick;
		std::string password;

		std::vector<std::string> admins,
								 ignored;

		std::vector< Server* > servers;

		std::vector<CommandInterface*> commands;
		std::vector<DynamicPluginLoading::DynamicPlugin*> dynamic_plugins;

		std::string recovery_password_sha256;

		/* For Stats: */
		std::chrono::time_point<std::chrono::system_clock> start_time;
		unsigned long long packets_received, packets_sent, commands_executed;

		std::mutex stat_mutex,
		           admin_mutex,
		           ignored_mutex,
				   commands_mutex,
				   dynamic_plugins_mutex;
				   // A servers_mutex is not needed since each server occupies its own thread and shares no data.

	public:

		/*
			A flag for add_a(...)
		*/
		enum class RELATIONSHIP {
			ADMIN,
			IGNORED
		};

		/* Constructor.

			@param n the bot's nick.
			@param pass the bot's nickserv password.
			@param _admins a vector of strings representing the
				current nicks of admins. Note: Admins may change their
				nicks once the bot comes on; the bot tracks NICK changes.
			@param sha256_recovery_pw [optional] the password to use to gain admin privs
				from an account with a different identity.

		*/
		Bot(const std::string& n, const std::string& pass, const std::vector<std::string>& _admins, const std::string& sha256_recovery_pw = "");

		/*
			Deallocates servers and commands.
		*/
		~Bot();

		/*
			Sets the sha256sum for a recovery password...

			@param rpw the sha256sum of the password.
		*/
		void set_recovery_sha256(const std::string& rpw);

		/* Adds a server.

			@param n the name of the server.
			@param a the address (irc.example.net)
			@param port the port as an integer.
			@param with_ssl whether or not to use
				SSL on the server. Defaults to using SSL.
		*/
		void add_server(std::string n , const std::string& a , const int& port, bool with_ssl = true);

		/*
			Connect to server by name

			@param server_name the server's name. This must be unique to work.
		*/
		void connect_server(const std::string& server_name);

		/*
			Disconnect from the server pointed to by s.

			@param s the server to disconnect from.

			TODO: Alternative means of disconnecting on the Bot
				level should be added.
		*/
		void disconnect_server(Server *s);

		/*
			Joins channel.

			@param server_name the server to send the JOIN to.
			@param channel_name the channel to join. This may be a #channel or a
				private message, but a private message wouldn't make much sense.

			@return the success of joining or not, to the best of the Bot's ability.
		*/
		bool join_channel(const std::string& server_name , const std::string& channel_name);

		/*
			Adds someone either to the IGNORE list or the ADMIN list.

			@param r the relationship (i.e., ADMIN, or IGNORED)
			@param user the user to relate to.
		*/
		void add_a(const RELATIONSHIP& r , const std::string& user);

		/*
			Reauthenticates the user by comparing hash_of_password to an internally-stored
			SHA256sum.

			@param user the user to reauthenticate. Upon success, the user is added to
				the admin list.
			@param hash_of_password a precalculated SHA256sum of the password submitted by
				the user.

			@return whether the reauthentication was successful.
		*/
		bool reauthenticate(const std::string& user, const std::string hash_of_password);

		/*
			Adds the CommandInterface cmd to the list of commands.

			@param cmd the command interface.
		*/
		void add_command( CommandInterface* cmd );

		/*
			Adds the dynamic command located at the path (absolute or relative)
				`name`.

			@param name the path/name of the shared library (.so) to dynamically
			load into the bot.

			This method also loads an instance of the shared library's implementation
			of IRC::CommandInterface into this->commands.
		*/
		void add_dynamic_command( const std::string& name );

		/*
			Removes the dynamic command stored in dynamic_plugins
			with the name `name`.

			@param name the path/name of the shared lib (.so) to dynamically
			unload from the bot.

			Note that this method also deletes and removes any commands that
			were instantiated using code from the `name` lib.
		*/
		void remove_dynamic_command( const std::string& name );

		/*
			Returns the vector of commands to the caller.

			The commands are immutable.
		*/
		std::vector<const CommandInterface *> get_commands(void) const;
		std::vector<std::string> get_stats(void);

		/* Reads from all connected servers and responds accordingly if
			a trigger is found.
		*/
		void listen();

	private:

		/*
			Helper function to perform the check for triggers and perform the
			appropriate actions on the appropriate channel on the right server.

			@param p the Packet to analyze.
		*/
		void _check_for_triggers(const Packet& p);

		/*
			Finds a server by name and returns the iterator.

			@param n the name of the server. This n is expected to be unique amongst
				all the servers.
			@return an iterator to the server named n, or std::vector::vector::end()
				if the server isn't found by the name n.
		*/
		std::vector<Server*>::iterator _find_server_by_name(const std::string& n);

		/*
			Verifies a person's presence in the admin list.

			@param person the person to look for in the admin list.
			@return whether or not the person is an admin.
		*/
		bool _is_admin(const std::string& person);

		/*
			Same as _is_admin but for ignored people
		*/
		bool _is_ignored(const std::string& person);

		void _listen_to_server(Server* s);

	};
};
#endif
