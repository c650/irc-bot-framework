#include "./include/googler.hpp" /* for write_to_string */
#include "./include/json.hpp"
#include "./include/url-shortener.hpp"

#include <iostream>
#include <fstream>
#include <string>

#include <curl/curl.h>

namespace ShortenURL {

	std::string uri_encode_url(const std::string& unformatted) {

		std::string result = "";

		CURL *curl = curl_easy_init();
		if (curl) {
			char *unformatted_raw = strdup(unformatted.data());

			char *escaped = curl_easy_escape(curl, unformatted_raw, strlen(unformatted_raw));

			result = escaped;

			curl_free(escaped);

			free(unformatted_raw);
			curl_easy_cleanup(curl);
		}

		return result;
	}

	std::string get_shortened_url(const std::string& long_url, const std::string& config_path) {

		std::string response = "";

		nlohmann::json api_info;
		try {
			std::fstream fs ( config_path, std::fstream::in);
			fs >> api_info;
			api_info = api_info["tinyurl"];
		} catch (...) {
			std::cerr << "File IO failed in ShortenURL. Couldn't retrieve API Key.\n";
			return response;
		}

		CURL* curl = curl_easy_init();
		if (curl) {
			std::string url = "http://tiny-url.info/api/v1/create?provider=tinyurl_com&apikey=" + api_info["api_key"].get<std::string>()
			                  + "&url=" + uri_encode_url(long_url);

			curl_easy_setopt(curl, CURLOPT_URL , url.data());

			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Googler::write_to_string);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

			curl_easy_perform(curl);
		}
		return response;
	}
};
