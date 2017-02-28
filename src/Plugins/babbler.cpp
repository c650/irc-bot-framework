/*
	babbler.cpp
*/

#include "../IRCBot/include/command-interface.hpp"
#include "../IRCBot/include/bot.hpp"

#include "./include/environment.hpp"
#include "./include/json.hpp"
#include "./include/random-line-stream.hpp"

class BabblerCommand : protected IRC::CommandInterface {

	RandomLineStream rls;

  public:

	BabblerCommand()
		: CommandInterface("@babble", "does a babble."), rls(ENVIRONMENT["babble"]["filepath"].get<std::string>()) {}

	void run(const IRC::Packet& p) {
		p.reply(rls.sample());
	}

};

extern "C" IRC::CommandInterface* maker(IRC::Bot* b = nullptr) {
	return (IRC::CommandInterface*)(new BabblerCommand);
}
