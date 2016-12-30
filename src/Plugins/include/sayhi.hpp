#ifndef _SAYHI_H
#define _SAYHI_H

#include "../../IRCBot/include/packet.hpp"
#include "../../IRCBot/include/command-interface.hpp"

class SayHi : public IRC::CommandInterface {

public:

	SayHi() : IRC::CommandInterface("@sayhi", "says hi.") {}

	bool triggered(const IRC::Packet& p) const {
		return p.type == IRC::Packet::PacketType::PRIVMSG && p.content.substr(0, this->trigger().length()) == this->trigger();
	}

	void run(const IRC::Packet& p) const {
		p.reply("Hello!");
	}
};

#endif
