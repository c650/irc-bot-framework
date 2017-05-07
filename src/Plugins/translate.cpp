/*
	translate.cpp
*/

#include "../IRCBot/include/command-interface.hpp"
#include "../IRCBot/include/bot.hpp"
#include "../IRCBot/include/packet.hpp"

#include <stdexcept>

#include "./include/json.hpp"
#include "./include/http.hpp"

/* taken from currency.cpp */
static void string_clean(std::string& str) {
	while(str.length() > 1 && str[0] == ' ')
		str = str.substr(1);
	if (str == " ") str = "";
}

class TranslateCommand : protected IRC::CommandInterface {

  public:

	TranslateCommand() : IRC::CommandInterface("@translate", "translates supplied text to english.") {}

	void run(const IRC::Packet& p) {
		std::string content = p.content.substr(this->trigger_string.length());
		string_clean(content);

		if (content.empty()) {
			p.reply("Nothing to translate.");
			return;
		}

		try {
			auto ret = MyHTTP::get("http://www.transltr.org/api/translate?text=" + MyHTTP::uri_encode(content) + "&to=en");
			std::cout << ret << '\n';
			auto json_obj = nlohmann::json::parse(ret);
			p.reply(json_obj["translationText"]);
		} catch (std::exception& e) {
			std::cerr << "Error in TranslateCommand::run(...): " << e.what() << '\n';
			p.reply("Experienced error while translating.");
		}
	}
};

extern "C" IRC::CommandInterface* maker(IRC::Bot* b = nullptr) {
	return (IRC::CommandInterface*)(new TranslateCommand);
}
