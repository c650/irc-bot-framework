#include "../IRCBot/include/command-interface.hpp"
#include "../IRCBot/include/bot.hpp"
#include "../IRCBot/include/packet.hpp"

#include "./include/http.hpp"
#include "./include/json.hpp"

#include <string>
#include <stdexcept>

static void strip_line_endings(std::string& str);
static void get_first_result(const std::string& query, std::string& res);

class UrbanCommand : protected IRC::CommandInterface {

  public:

	UrbanCommand()
		: CommandInterface("@urban ", "checks urban dictionary for a definition.") {}

	void run(const IRC::Packet& p) {
		std::string def = "";

		get_first_result(p.content.substr(this->trigger_string.length()) , def);

		if (def.length() >= 420) {
			p.reply("Grr. Why do you make me work, " + p.sender + "??");
			for (size_t i = 0; i < def.length(); i += 420) {
				p.owner->privmsg(p.sender , def.substr(i, 420));
			}

		} else if (!def.empty()) {
			p.reply(def);
		}
	}

};

static void get_first_result(const std::string& query, std::string& res) {

	std::string response = "";
	if ( !MyHTTP::get("http://api.urbandictionary.com/v0/define?term=" + MyHTTP::uri_encode(query) , response ) ) {
		res = "";
		return;
	}

	try {
		nlohmann::json data = nlohmann::json::parse(response);
		if (data["result_type"] == "exact") {
			res = data["list"].at(0)["definition"];
			strip_line_endings(res);
		}
	} catch (std::exception& e) {
		std::cerr << "Failed in Urban::get_first_result: " << e.what() << '\n';
		res = "";
	}
}

static void strip_line_endings(std::string& str) {
	size_t loc = 0;
	while ( (loc = str.find("\r\n", loc) ) != std::string::npos) {
		str.replace(loc, 2, " ");
	}
}

extern "C" {

	IRC::CommandInterface* maker(IRC::Bot *b = nullptr) {
		return (IRC::CommandInterface*)(new UrbanCommand);
	}

};
