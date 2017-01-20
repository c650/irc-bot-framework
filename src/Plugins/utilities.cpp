/*
	sayhi.cpp
*/

#include "../IRCBot/include/command-interface.hpp"
#include "../IRCBot/include/bot.hpp"
#include "../IRCBot/include/packet.hpp"

class UtilityCommands : protected IRC::CommandInterface {

  public:

	UtilityCommands(IRC::Bot *b = nullptr) : IRC::CommandInterface("@kick, @join, @part, @quit, @nick, @adda", "does utility actions", b, true) {
		if (b == nullptr) {
			throw std::logic_error("In UtilityCommands, Bot *b cannot be nullptr!");
		}
	}

	bool triggered(const IRC::Packet& p) {
		std::string cmd = p.content.substr(0,5);
		return p.type == IRC::Packet::PacketType::PRIVMSG
			&& ( cmd == "@kick" || cmd == "@join" || cmd == "@part" || cmd == "@quit" || cmd == "@nick" || cmd == "@adda");
	}

	void run(const IRC::Packet& p) {
		std::string cmd = p.content.substr(0,5);

		if (p.content.length() > 5) {
			if ( cmd == "@kick" ) {
				p.owner->kick(p.channel, p.content.substr(6));
			} else if ( cmd == "@join" ) {
				p.owner->join_channel( p.content.substr(6 , p.content.find(" ", 6) - 6) );
			} else if ( cmd == "@part" ) {
				p.owner->part_channel( p.content.substr(6) );
			} else if ( cmd == "@nick" ) {
				p.owner->set_nick( p.content.substr(6) );
			} else if ( cmd == "@adda" ) {
				std::vector<std::string> args;
				p.get_args(args);
				if (args.size() < 3) {
					p.owner->privmsg(p.sender, "Error. Usage: @adda TYPE name");
					return;
				}

				try {
					std::transform(args[1].begin(), args[1].end(), args[1].begin(), ::tolower);
				} catch (...) {
					std::cerr << "failed in UtilityCommands @adda to transform to lowercase : " << args[1] << '\n';
					return;
				}
				IRC::Bot::RELATIONSHIP r = IRC::Bot::RELATIONSHIP::IGNORED;
				if (args[1] == "admin") {
					r = IRC::Bot::RELATIONSHIP::ADMIN;
				} else if (args[1] != "ignore") { // all else except "admin" and "ignore" (case insensitive)
					p.owner->privmsg(p.sender, "Error. Invalid argument: " + args[1]);
					return;
				}

				bot_ptr->add_a(r , args[2]);
				p.reply("Added " + args[2] + " to " + args[1] + " list.");
			}
		}

		if (cmd == "@quit") { // quit
			p.owner->disconnect( p.content.length() > 6 ? p.content.substr(6) : "Quitting..." );
		}
	}
};

extern "C" {

	IRC::CommandInterface* maker(IRC::Bot *b = nullptr) {
		return (IRC::CommandInterface*)(new UtilityCommands(b));
	}

};
