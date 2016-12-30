#include "./include/iplookup.hpp"
#include "./include/json.hpp"
#include "./include/http.hpp"

#include <stdexcept>

namespace IPLookup {

	static void do_lookup_internal(const std::string& host, nlohmann::json& result) {
		std::string response = "";
		if ( !MyHTTP::get("http://freegeoip.net/json/" + host, response) ) {
			return;
		}

		try {
			result = nlohmann::json::parse(response);
		} catch (...) {
			std::cerr << "Couldn't parse: " << response << '\n';
		}
	}

	std::string do_lookup(const std::string& host) {

		try {
			std::cout << "Looking up: " << host << '\n';

			nlohmann::json result;
			do_lookup_internal(host,result);

			return "Location: " + result["city"].get<std::string>() + ", " + result["region_code"].get<std::string>() + ", "
			       		+ result["country_name"].get<std::string>();
		} catch (std::exception& e) {
			std::cerr << e.what() << '\n';
			return "Lookup failed.";
		}
	}

};