#ifndef _BOT_UTILITY_H
#define _BOT_UTILITY_H

#include "../../IRCBot/include/command-interface.hpp"
#include "../../IRCBot/include/packet.hpp"

class Utility : public IRC::CommandInterface {

public:

	Utility() : IRC::CommandInterface("@kick,@join,@part,@quit", "does utility actions", true) {}

	bool triggered(const IRC::Packet& p) {
		std::string cmd = p.content.substr(0,5);
		return p.type == IRC::Packet::PacketType::PRIVMSG
			&& ( cmd == "@kick" || cmd == "@join" || cmd == "@part" || cmd == "@quit" );
	}

	void run(const IRC::Packet& p) {
		std::string cmd = p.content.substr(0,5);

		if (p.content.length() > 5) {
			if ( cmd == "@kick" ) {
				p.owner->kick(p.channel, p.content.substr(6));p.owner->kick(p.channel, p.content.substr(6));
			} else if ( cmd == "@join" ) {
				p.owner->join_channel( p.content.substr(6 , p.content.find(" ", 6) - 6) );
			} else if ( cmd == "@part" ) {
				p.owner->part_channel( p.content.substr(6) );
			}
		}

		if (cmd == "@quit") { // quit
			p.owner->disconnect( p.content.length() > 6 ? p.content.substr(6) : "Quitting..." );
		}
	}
};

#endif
