#include <iostream>
#include <fstream>
#include <string>

#include "./include/googler.hpp"
#include "./include/json.hpp"
#include "./include/url-shortener.hpp" /* shortening urls */
#include "./include/http.hpp"

namespace Googler {

	void do_google_search(const std::string& query,
	                      const size_t& num_res_to_show,
						  std::vector<std::string>& res_vec,
						  const std::string& config_path)
	{
		// requires a JSON file with google API stuff
		nlohmann::json api_info;
		try {
			std::fstream fs( config_path , std::fstream::in);
			fs >> api_info;
			fs.close();
			api_info = api_info["google_search"]; // ignore other API info
		} catch (...) {
			std::cerr << "File IO failed in Googler. Couldn't retrieve API Key.\n";
			return;
		}

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
			                   + ShortenURL::get_shortened_url(item["link"].get<std::string>(), config_path));
		}
	}
};
