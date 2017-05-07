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
	auto it = std::remove_if(str.begin(), str.end(), [](const char& c){
		return std::isspace(c);
	});
	str.resize(std::distance(str.begin(),it));
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
			auto json_obj = nlohmann::json::parse(MyHTTP::get("http://www.transltr.org/api/translate?text=" + content + "&to=en"));
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
