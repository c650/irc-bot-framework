#include "./include/http.hpp" /* for write_to_string */
#include "./include/json.hpp"
#include "./include/url-shortener.hpp"

#include <iostream>
#include <fstream>
#include <string>

#include <curl/curl.h>

namespace ShortenURL {

	std::string get_shortened_url(const std::string& long_url, const std::string& config_path) {

		std::string response = "";

		nlohmann::json api_info;
		try {
			std::fstream fs ( config_path, std::fstream::in);
			fs >> api_info;
			fs.close();
			api_info = api_info["tinyurl"];
		} catch (...) {
			std::cerr << "File IO failed in ShortenURL. Couldn't retrieve API Key.\n";
			return response;
		}

		std::string url = "http://tiny-url.info/api/v1/create?provider=tinyurl_com&apikey=" + api_info["api_key"].get<std::string>()
						  + "&url=" + MyHTTP::uri_encode(long_url);

		MyHTTP::get(url, response);

		return response;
	}
};
