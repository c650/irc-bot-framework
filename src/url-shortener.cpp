#include "./include/http.hpp" /* for write_to_string */
#include "./include/json.hpp"
#include "./include/url-shortener.hpp"

#include "./include/environment.hpp"
#include <iostream>
#include <fstream>
#include <string>

#include <curl/curl.h>

namespace ShortenURL {

	std::string get_shortened_url(const std::string& long_url) {

		std::string response = "";

		std::string url = "http://tiny-url.info/api/v1/create?provider=tinyurl_com&apikey="
		                  + ENVIRONMENT["tinyurl"]["api_key"].get<std::string>()
						  + "&url=" + MyHTTP::uri_encode(long_url);

		MyHTTP::get(url, response);

		return response;
	}
};
