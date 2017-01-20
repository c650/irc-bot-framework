/*
	speak.cpp
*/

#include "../IRCBot/include/command-interface.hpp"
#include "../IRCBot/include/bot.hpp"
#include "../IRCBot/include/packet.hpp"

class SpeakCommand : protected IRC::CommandInterface {

  public:

	SpeakCommand() : IRC::CommandInterface("@speak ", "says a message to channel: @speak <chan> <msg...>", nullptr, true) {}

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

extern "C" {

	IRC::CommandInterface* maker(IRC::Bot* b = nullptr) {
		return (IRC::CommandInterface*)(new SpeakCommand);
	}

};
