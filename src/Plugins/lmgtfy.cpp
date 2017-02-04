/*
	lmgtfy.cpp
*/

#include "../IRCBot/include/command-interface.hpp"
#include "../IRCBot/include/bot.hpp"
#include "../IRCBot/include/packet.hpp"

#include "./include/http.hpp"

#include <algorithm>

class LMGTFYCommand : protected IRC::CommandInterface {

  public:

	LMGTFYCommand()
		: CommandInterface("@lmgtfy ", "mean way to tell ppl to google things.") {}

	void run(const IRC::Packet& p) {
		std::string query = p.content.substr(this->trigger().length());

		auto it = std::find_if(query.begin(),query.end(), [](const char& c){
			return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c >= '0' && c <= '9';
		});

		if (it == query.end()) {
			p.reply("You supplied no meaningful search!");
		} else {
			p.reply("http://lmgtfy.com/?q=" + MyHTTP::uri_encode( query ) );
		}
	}

};

extern "C" {

	IRC::CommandInterface* maker(IRC::Bot* b = nullptr) {
		return (IRC::CommandInterface*)(new LMGTFYCommand);
	}

};
