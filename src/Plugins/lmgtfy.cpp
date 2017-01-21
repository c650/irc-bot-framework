/*
	lmgtfy.cpp
*/

#include "../IRCBot/include/command-interface.hpp"
#include "../IRCBot/include/bot.hpp"
#include "../IRCBot/include/packet.hpp"

#include "./include/http.hpp"

class LMGTFYCommand : protected IRC::CommandInterface {

  public:

	LMGTFYCommand()
		: CommandInterface("@lmgtfy ", "mean way to tell ppl to google things.") {}

	void run(const IRC::Packet& p) {
		p.reply("http://lmgtfy.com/?q=" + MyHTTP::uri_encode( p.content.substr(this->trigger().length()) ) );
	}

};

extern "C" {

	IRC::CommandInterface* maker(IRC::Bot* b = nullptr) {
		return (IRC::CommandInterface*)(new LMGTFYCommand);
	}

};
