/*
	slap.cpp
*/

#include "../IRCBot/include/command-interface.hpp"
#include "../IRCBot/include/bot.hpp"
#include "../IRCBot/include/packet.hpp"

class SlapCommand : protected IRC::CommandInterface {

  public:

	SlapCommand() : IRC::CommandInterface("@slap ", "slaps arguments.") {}

	void run(const IRC::Packet& p) {
		p.reply("\01ACTION slapped the hell outta " + p.content.substr(this->trigger().length()) + "\01");
	}

};

extern "C" {

	IRC::CommandInterface* maker(IRC::Bot* b = nullptr) {
		return (IRC::CommandInterface*)(new SlapCommand);
	}

};
