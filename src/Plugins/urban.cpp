#include "../IRCBot/include/command-interface.hpp"
#include "../IRCBot/include/bot.hpp"
#include "../IRCBot/include/packet.hpp"

#include "./include/http.hpp"
#include "./include/json.hpp"

#include <string>
#include <stdexcept>

static void strip_line_endings(std::string& str);
static std::string get_first_result(const std::string& query);

class UrbanCommand : protected IRC::CommandInterface {

  public:

	UrbanCommand()
		: CommandInterface("@urban ", "checks urban dictionary for a definition.") {}

	void run(const IRC::Packet& p) {

		std::string query = p.content.substr(this->trigger_string.length());

		std::string def = get_first_result( query );

		if (def.length() >= 420) {
			p.reply("Grr. See for yourself. https://www.urbandictionary.com/define.php?term=" + MyHTTP::uri_encode(query));
		} else if (!def.empty()) {
			p.reply(def);
		}
	}

};

static std::string get_first_result(const std::string& query) {
	std::string res = "Unknown.";
	try {
		nlohmann::json data = nlohmann::json::parse(MyHTTP::get("http://api.urbandictionary.com/v0/define?term=" + MyHTTP::uri_encode(query)));
		if (data["result_type"] == "exact") {
			res = data["list"].at(0)["definition"];

			std::string example = data["list"].at(0)["example"].get<std::string>();
			if (!example.empty())
				res += " [*] Ex. \x1D" + example + "\x1D";

			strip_line_endings(res);
		}
	} catch (std::exception& e) {
		std::cerr << "Failed in Urban::get_first_result: " << e.what() << '\n';
	}
	return res;
}

static void strip_line_endings(std::string& str) {
	size_t loc = 0;
	while ( (loc = str.find("\r\n", loc) ) != std::string::npos) {
		str.replace(loc, 2, " ");
	}
}

extern "C" IRC::CommandInterface* maker(IRC::Bot *b = nullptr) {
	return (IRC::CommandInterface*)(new UrbanCommand);
}
