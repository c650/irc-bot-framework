#include "../IRCBot/include/command-interface.hpp"
#include "../IRCBot/include/bot.hpp"

#include "./include/stocks.hpp"
#include "./include/http.hpp"
#include "./include/json.hpp"

#include <stdexcept>

static std::string get_stock_summary(const std::string& q);

class StocksCommand : protected IRC::CommandInterface {

	unsigned long long queries_done;

  public:

	StocksCommand() : CommandInterface("@stock ", "checks a stock ticker price."), queries_done(0) {}

	void run(const IRC::Packet& p) {
		p.reply(get_stock_summary( p.content.substr(this->trigger().length()) ));
		this->queries_done++;
	}

	std::string get_stats(void) const {
		return "Stock Queries Done: " + std::to_string(queries_done);
	}

};

static std::string get_stock_summary(const std::string& q) {
	try {
		std::string response = "";

		MyHTTP::get("https://www.google.com/finance/info?q=" + MyHTTP::uri_encode(q.substr(0, q.find(" ")) ), response);

		response.erase(0, 5); // erase first five characters.
		response.erase(response.length()-2); // and last two characters.
		nlohmann::json data = nlohmann::json::parse(response);

		return data["t"].get<std::string>() + " on " + data["e"].get<std::string>()
				+ " was last at: $" + data["l"].get<std::string>()
				+ ". Percent Day Change: " + data["cp"].get<std::string>();
	} catch (std::exception& e) {
		std::cerr << "Failed in get_stock_summary(): " << e.what() << '\n';
	}

	return "Failed to look up: " + q;
}

extern "C" {

	IRC::CommandInterface* maker(IRC::Bot* b = nullptr) {
		return (IRC::CommandInterface*)(new StocksCommand);
	}

};
