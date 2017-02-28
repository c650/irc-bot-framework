#include "../IRCBot/include/command-interface.hpp"
#include "../IRCBot/include/bot.hpp"
#include "../IRCBot/include/packet.hpp"

#include "./include/json.hpp"
#include "./include/http.hpp"

#include <string>
#include <stdexcept>

static void do_lookup_internal(const std::string& host, nlohmann::json& result);
static std::string do_lookup(const std::string& host);

class IPLookupCommand : protected IRC::CommandInterface {

  public:

	IPLookupCommand() : IRC::CommandInterface("@iplookup ", "looks up IP address.", nullptr, true) {}

	void run(const IRC::Packet& p) {
		p.reply(do_lookup(p.content.substr(this->trigger().length())));
	}
};

static void do_lookup_internal(const std::string& host, nlohmann::json& result) {
	try {
		result = nlohmann::json::parse(MyHTTP::get("http://freegeoip.net/json/" + host));
	} catch (std::exception& e) {
		std::cerr << "IPLookup Failure in do_lookup_internal: " << e.what() << '\n';
	}
}

static std::string do_lookup(const std::string& host) {

	if (host.empty())
		return "Error: Needs argument.";

	try {
		std::cout << "Looking up: " << host << '\n';

		nlohmann::json result;
		do_lookup_internal(host,result);

		return "Location: " + result["city"].get<std::string>() + ", " + result["region_code"].get<std::string>() + ", "
		       		+ result["country_name"].get<std::string>();
	} catch (std::exception& e) {
		std::cerr << e.what() << '\n';
		return "Lookup of " + host + " failed.";
	}
}

extern "C" IRC::CommandInterface* maker(IRC::Bot *b = nullptr) {
	return (IRC::CommandInterface*)(new IPLookupCommand);
}
