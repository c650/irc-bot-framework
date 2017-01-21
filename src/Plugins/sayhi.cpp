/*
	sayhi.cpp
*/

#include "../IRCBot/include/command-interface.hpp"
#include "../IRCBot/include/bot.hpp"
#include "../IRCBot/include/packet.hpp"

class SayHiCommand : protected IRC::CommandInterface {

  public:

	SayHiCommand() : IRC::CommandInterface("@sayhi", "says hi.") {}

	void run(const IRC::Packet& p) {
		p.reply("Hello!");
	}
};

extern "C" {

	IRC::CommandInterface* maker(IRC::Bot* b = nullptr) {
		return (IRC::CommandInterface*)(new SayHiCommand);
	}

};
