/*
	bitcoin.cpp

	checks the price of bitcoin.
*/

#include "../IRCBot/include/command-interface.hpp"
#include "../IRCBot/include/bot.hpp"
#include "../IRCBot/include/packet.hpp"

#include "./include/json.hpp"
#include "./include/http.hpp"

class BitcoinCommand : protected IRC::CommandInterface {

  public:

	BitcoinCommand() : IRC::CommandInterface("@btc", "gets the current bitcoin price") {}

	void run(const IRC::Packet& p) {
		try {
			nlohmann::json data = nlohmann::json::parse(MyHTTP::get("http://api.coindesk.com/v1/bpi/currentprice.json"));

			p.reply("$" + data["bpi"]["USD"]["rate"].get<std::string>() + " as of: " + data["time"]["updated"].get<std::string>());

		} catch (std::exception& e) {
			std::cerr << "Error in BitcoinCommand: " << e.what() << '\n';
			p.reply("Sorry. I failed to fetch data.");
		}
	}
};

extern "C" {
	IRC::CommandInterface* maker(IRC::Bot* b = nullptr) {
		return (IRC::CommandInterface*)(new BitcoinCommand);
	}
};
