#ifndef _BABBLER_H
#define _BABBLER_H

#include "../../IRCBot/include/command-interface.hpp"
#include "../../IRCBot/include/packet.hpp"

#include "./random-line-stream.hpp"

class Babbler : public IRC::CommandInterface {

	RandomLineStream rls;

public:

	Babbler(const std::string& babbles_filepath)
		: CommandInterface("@babble", "does a babble."), rls(babbles_filepath) {}

	bool triggered(const IRC::Packet& p) {
		return p.type == IRC::Packet::PacketType::PRIVMSG && p.content.substr(0,this->trigger().length()) == this->trigger();
	}

	void run(const IRC::Packet& p) {
		p.reply(rls.sample());
	}

};

#endif
