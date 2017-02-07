/*
	currency.cpp
*/

#include "../IRCBot/include/command-interface.hpp"
#include "../IRCBot/include/bot.hpp"
#include "../IRCBot/include/packet.hpp"

#include "./include/json.hpp"
#include "./include/http.hpp"

#include <algorithm>
#include <cctype>

static void string_clean(std::string& str);

class CurrencyCommand : protected IRC::CommandInterface {

  public:

	CurrencyCommand() : IRC::CommandInterface("@currency ", "Get conversion rate.") {}

	void run(const IRC::Packet& p) {
		std::string command = p.content.substr(this->trigger_string.length());

		/* this will expect "@currency BASE OTHER,OTHER,OTHER"*/
		try {
			std::string base = command.substr(0, command.find(" "));
			std::string others = command.substr(command.find(" ")+1);

			/* capitalize all base/other symbols, remove any whitespaces*/
			string_clean(base);
			string_clean(others);

			if (others.empty())
				return;

			std::string api_query = "https://api.fixer.io/latest?base=" + base + "&symbols=" + others;

			std::string response = "";
			MyHTTP::get(api_query, response);
			nlohmann::json result = nlohmann::json::parse(response);

			response = "As of " + result["date"].get<std::string>() + ", 1.00 " + result["base"].get<std::string>()
						+ " will get you: ";

			std::stringstream ss;
			bool got_res = false;
			for (nlohmann::json::iterator it = result["rates"].begin(); it != result["rates"].end(); ++it) {
				got_res = true;
				ss << it.key() << " -> " << it.value() << ", ";
			}

			if (got_res) {
				std::string tmp = ss.str();
				response += tmp.substr(0, tmp.rfind(","));
				p.reply(response);
			}

		} catch (std::exception& e) {
			std::cerr << "Failed to answer currency query of " << command << ". Error: " << e.what() << '\n';
			p.reply("Sorry, I couldn't look up: " + command);
		}
	}
};

static void string_clean(std::string& str) {
	auto it = std::remove_if(str.begin(), str.end(), [](const char& c){
		return std::isspace(c);
	});
	str.resize(std::distance(str.begin(),it));
	for (auto& c : str)
		c = std::toupper(c);
}

extern "C" {

	IRC::CommandInterface* maker(IRC::Bot* b = nullptr) {
		return (IRC::CommandInterface*)(new CurrencyCommand);
	}

};
