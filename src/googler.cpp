#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

#include "./include/environment.hpp"
#include "./include/googler.hpp"
#include "./include/json.hpp"
#include "./include/url-shortener.hpp" /* shortening urls */
#include "./include/http.hpp"

namespace Googler {

	void do_google_search(const std::string& query, const size_t& num_res_to_show, std::vector<std::string>& res_vec) {

		try {
			nlohmann::json api_info = ENVIRONMENT["google_search"];

			std::string response;
			MyHTTP::get(api_info["address"].get<std::string>()
						+ "key=" + api_info["key"].get<std::string>()
						+ "&cx=" + api_info["cx"].get<std::string>()
						+ "&q=" + MyHTTP::uri_encode(query), response);

			nlohmann::json search_result = nlohmann::json::parse(response);

			size_t count = search_result["items"].size() > num_res_to_show ? num_res_to_show : search_result["items"].size();

			nlohmann::json item;
			for (size_t i = 0; i < count; ++i) {
				item = search_result["items"].at(i);

				res_vec.push_back( item["title"].get<std::string>() + "->"
				                   + ShortenURL::get_shortened_url(item["link"].get<std::string>()));
			}
		} catch (std::exception& e) {
			std::cerr << e.what() << '\n';
		}
	}
};
