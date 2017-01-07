#include "./include/urban.hpp"
#include "./include/http.hpp"
#include "./include/json.hpp"

#include <string>
#include <stdexcept>

namespace Urban {

	static void strip_line_endings(std::string& str);

	void get_first_result(const std::string& query, std::string& res) {

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

};
