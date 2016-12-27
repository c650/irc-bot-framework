#include "./include/iplookup.hpp"
#include "./include/json.hpp"
#include "./include/http.hpp"

namespace IPLookup {

	static void do_lookup_internal(const std::string& host, nlohmann::json& result) {
		std::string response = "";
		MyHTTP::get("http://freegeoip.net/json/" + host, response);
		result = nlohmann::json::parse(response);
	}

	std::string do_lookup(const std::string& host) {

		nlohmann::json result;
		do_lookup_internal(host,result);

		return "Location: " + result["city"].get<std::string>() + ", " + result["region_code"].get<std::string>() + ", "
		       		+ result["country_name"].get<std::string>();
	}

};
