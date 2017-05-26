#include "../IRCBot/include/command-interface.hpp"
#include "../IRCBot/include/bot.hpp"
#include "../IRCBot/include/packet.hpp"

#include "./include/http.hpp"
#include "./include/json.hpp"

#include <string>
#include <stdexcept>

static void strip_line_endings(std::string& str);
static std::string get_nth_result(const std::string& query, size_t n);

class UrbanCommand : protected IRC::CommandInterface {

	static const size_t MAX_DEF_LEN = 420; /* Maximum character length of definition and example. */

  public:

	UrbanCommand()
		: CommandInterface("@urban ", "checks urban dictionary for a definition.") {}

	void run(const IRC::Packet& p) {

		std::string query = p.content.substr(this->trigger_string.length());

		size_t n = 1, loc;
		if ((loc = query.find(" ")) != std::string::npos) {
			try {
				n = (size_t)std::stoi(query);
				query = query.substr(loc + 1);
			} catch (...) {
				n = 1;
			}
		}

		// std::string def = get_first_result( query );
		std::string def = get_nth_result( query , n );

		if (def.length() >= MAX_DEF_LEN) {
			p.reply("Grr. See for yourself. https://www.urbandictionary.com/define.php?term=" + MyHTTP::uri_encode(query));
		} else if (!def.empty()) {
			p.reply(def);
		}
	}

};

static std::string get_nth_result(const std::string& query, size_t n) {
	std::string res = "Unknown.";

	if (n != 0) /* move n from 1-based to 0-based. */
		--n;    /* but only if n isn't 0 already (avoid underflow) */

	try {
		nlohmann::json data = nlohmann::json::parse(MyHTTP::get("http://api.urbandictionary.com/v0/define?term=" + MyHTTP::uri_encode(query)));
		if (data["result_type"] == "exact") {
			if (n >= data["list"].size()) {
				res = "No " + n;
				res += "th entry for " + query; // one-liner wasn't working :|
				return res;
			}
			res = data["list"].at(n)["definition"];

			std::string example = data["list"].at(n)["example"].get<std::string>();
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
