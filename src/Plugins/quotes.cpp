#include <stdexcept>
#include <string>

#include "./include/quotes.hpp"
#include "./include/json.hpp"
#include "./include/http.hpp"

namespace Quotes {

	std::string get_random_quote(void) {

		try {
			std::string response = "";
			MyHTTP::get("https://favqs.com/api/qotd", response);

			nlohmann::json data = nlohmann::json::parse(response);

			std::cout << data << '\n';

			return data["quote"]["body"].get<std::string>() + " -- " + data["quote"]["author"].get<std::string>();

		} catch (std::exception& e) {
			std::cerr << "Failed in get_random_quote(): " << e.what() << '\n';
		}
		return "Couldn't grab a quote!";
	}

}
