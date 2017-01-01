#include "./include/http.hpp"
#include "./include/json.hpp"

namespace Stocks {

	std::string get_stock_summary(const std::string& q) {

		std::string response = "";

		if ( MyHTTP::get("https://www.google.com/finance/info?q=" + q.substr(0, q.find(" ")), response) ) {
			response.erase(0, 5); // erase first five characters.
			response.erase(response.length()-2); // and last two characters.
			nlohmann::json data = nlohmann::json::parse(response);

			return data["t"].get<std::string>() + " on " + data["e"].get<std::string>()
					+ " was last at: $" + data["l"].get<std::string>()
					+ ". Percent Day Change: " + data["cp"].get<std::string>();
		}

		return "Failed to look up: " + q;
	}

}
