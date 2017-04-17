/*
	joke.cpp
*/

#include "../IRCBot/include/command-interface.hpp"
#include "../IRCBot/include/bot.hpp"
#include "../IRCBot/include/packet.hpp"

#include "./include/json.hpp"
#include "./include/http.hpp"

#include <stdexcept>

class JokeCommand : protected IRC::CommandInterface {

  public:

	JokeCommand() : IRC::CommandInterface("@joke", "yields a joke.") {}

	void run(const IRC::Packet& p) {
		try {
			nlohmann::json joke = nlohmann::json::parse(MyHTTP::get("http://tambal.azurewebsites.net/joke/random"));
			p.reply(joke["joke"].get<std::string>());
		} catch (std::exception& e) {
			std::cerr << "JokeCommand::run() error: " << e.what() << '\n';
			p.reply("Whoops. Couldn't fetch a joke. That's funny.");
		}
	}
};

extern "C" IRC::CommandInterface* maker(IRC::Bot* b = nullptr) {
	return (IRC::CommandInterface*)(new JokeCommand);
}
