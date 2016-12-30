#ifndef _SPEAK_H
#define _SPEAK_H

#include "../../IRCBot/include/command-interface.hpp"
#include "../../IRCBot/include/packet.hpp"

class Speak : public IRC::CommandInterface {

public:

	Speak() : IRC::CommandInterface("@speak ", "says a message to channel: @speak <chan> <msg...>", true) {}

	bool triggered(const IRC::Packet& p) {
		return p.type == IRC::Packet::PacketType::PRIVMSG && p.content.substr(0,this->trigger().length()) == this->trigger();
	}

	void run(const IRC::Packet& p) {
		std::string arguments = p.content.substr(this->trigger().length()); // everything after "@speak "

		size_t first_space_idx = arguments.find(" ");
		if (first_space_idx == std::string::npos)
			return;
		std::string chan = arguments.substr(0, first_space_idx);
		std::string msg = arguments.substr(first_space_idx	+ 1);

		p.owner->privmsg( chan , msg );
	}

};

#endif
