/*
	quotes.cpp
*/
#include "../IRCBot/include/command-interface.hpp"
#include "../IRCBot/include/bot.hpp"
#include "../IRCBot/include/packet.hpp"

#include "./include/json.hpp"
#include "./include/http.hpp"

#include <stdexcept>
#include <string>

#include <iostream>

static std::string get_random_quote(void);

class QuoteCommand : protected IRC::CommandInterface {
  public:
	QuoteCommand() : CommandInterface("@quote", "delivers the quote of the day.") {}

	void run(const IRC::Packet& p) {
		p.reply(get_random_quote());
	}
};

static std::string get_random_quote(void) {

	try {
		std::string response = "";
		MyHTTP::get("https://favqs.com/api/qotd", response);

		nlohmann::json data = nlohmann::json::parse(response);

		return data["quote"]["body"].get<std::string>() + " -- " + data["quote"]["author"].get<std::string>();

	} catch (std::exception& e) {
		std::cerr << "Failed in get_random_quote(): " << e.what() << '\n';
	}
	return "Couldn't grab a quote!";
}

extern "C" {

	IRC::CommandInterface* maker(IRC::Bot* b = nullptr) {
		return (IRC::CommandInterface*)(new QuoteCommand);
	}

};
