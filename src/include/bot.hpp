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

		*/
		Bot(const std::string& n, const std::string& pass, const std::vector<std::string>& _admins);

		/*
			Deallocates servers and commands.
		*/
		~Bot();

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
			on some trigger, use function f.

			f has to be:
				void f(const Packet& p);

			@param trigger the string to look for. The trigger must appear
				at the beginning of the message being inspected for it to be a valid command.
			@param f the function to call upon triggering the command.
			@param desc a description of the command. This is used in @help
			@param requires_admin whether or not the command is privileged/sensitive and
				therefore requires administrative rights to run.
		*/
		template <class Func>
		void on_privmsg(const std::string& trigger, Func f, const std::string& desc, bool requires_admin = false);

		/*
			on_join, on_kick, and on_nick have not yet been implemented.
		*/
		// template <class Func>
		// void on_join(Func f);
		//
		// template <class Func>
		// void on_kick(Func f);

		// void on_nick();

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

		void connect_server(const std::vector<Server*>::iterator& it);
		bool join_channel(const std::vector<Server*>::iterator& it, const std::string& channel_name);

	};

	template <class Func>
	void Bot::on_privmsg(const std::string& trigger, Func f, const std::string& desc, bool requires_admin) {
		commands.push_back( new Command(trigger, f , desc, requires_admin) );
	}
};
#endif
