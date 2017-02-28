/*
	1337.cpp
*/
#include "../IRCBot/include/command-interface.hpp"
#include "../IRCBot/include/bot.hpp"

class EliteCommand : protected IRC::CommandInterface {

	const std::string reglr = "abeiolgsqtz";
	const std::string elite = "48310195972";

public:

	EliteCommand() : CommandInterface("@1337 ", "translates to and from 13375p34k.") {}

	void run(const IRC::Packet& p) {
		std::string sub = p.content.substr(this->trigger_string.length());

		std::string msg = "";

		try {
			msg = sub.substr(sub.find(" "));
		} catch (std::exception& e) {
			std::cerr << "EliteCommand::run() error: " << e.what() << '\n';
			p.reply("Error. Usage: @1337 [to | from] [message...]");
		}

		sub = sub.substr(0, sub.find(" "));
		if (sub == "from") {
			for (auto& e : msg) {
				for (size_t i = 0; i < elite.length(); ++i) {
					if (std::tolower(e) == elite[i])
						e = reglr[i];
				}
			}
		} else {
			for (auto& e : msg) {
				for (size_t i = 0; i < reglr.length(); ++i) {
					if (std::tolower(e) == reglr[i])
						e = elite[i];
				}
			}
		}
		p.reply(msg);
	}
};

extern "C" IRC::CommandInterface* maker(IRC::Bot* b = nullptr) {
	return (IRC::CommandInterface*)(new EliteCommand);
}
