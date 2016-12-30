#include "./include/http.hpp" /* for write_to_string */
#include "./include/json.hpp"
#include "./include/url-shortener.hpp"

#include "./include/environment.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

#include <curl/curl.h>

namespace ShortenURL {

	std::string get_shortened_url(const std::string& long_url) {

		std::string response = "";

		try {
			std::string url = "http://tiny-url.info/api/v1/create?provider=tinyurl_com&apikey="
			                  + ENVIRONMENT["tinyurl"]["api_key"].get<std::string>()
							  + "&url=" + MyHTTP::uri_encode(long_url);

			MyHTTP::get(url, response);
		} catch (std::exception& e) {
			std::cerr << "Failed to shorten the URL " << long_url << " because: " << e.what() << '\n';
			response = ""; // just in case.
		}

		return response;
	}
};
