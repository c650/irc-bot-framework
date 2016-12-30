#ifndef _SLAP_H
#define _SLAP_H

#include "../../IRCBot/include/command-interface.hpp"
#include "../../IRCBot/include/packet.hpp"

class Slap : public IRC::CommandInterface {

public:

	Slap() : IRC::CommandInterface("@slap ", "slaps arguments.") {}

	bool triggered(const IRC::Packet& p) {
		return p.type == IRC::Packet::PacketType::PRIVMSG && p.content.substr(0,this->trigger().length()) == this->trigger();
	}

	void run(const IRC::Packet& p) {
		p.reply("\001ACTION slapped the hell outta " + p.content.substr(this->trigger().length()) + "\001");
	}

};

#endif
