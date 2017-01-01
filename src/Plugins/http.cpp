#include <iostream>
#include <curl/curl.h>
#include <cstring>
#include <stdexcept>

#include "./include/json.hpp"

namespace MyHTTP {

	/* to store result of GET in stream */
	static size_t write_to_string(void *ptr, size_t size, size_t nmemb, std::string& stream) {
		size_t realsize = size * nmemb;
		std::string temp(static_cast<const char*>(ptr), realsize);
		stream.append(temp);
		return realsize;
	}

	static void _parse_params(const nlohmann::json& params, std::string& p) {
		// try {
		// 	for (auto element = params.begin(); element != params.end() ; ++element) {
		// 		p.append((*element).key().get<std::string>() + "=" + (*element).value().get<std::string>() + "&");
		// 	}
		// 	p.pop_back(); // remove last '&'
		// } catch (std::exception& e) {
		// 	std::cerr << "Failed to parse params: " << e.what() << '\n';
		// 	p = "";
		// }
		p="";
	}

	/* see .hpp for docs */
	bool get(const std::string& url, std::string& response) {

		std::cout << "MyHTTP::get( " << url << " )\n";

		CURL *curl = curl_easy_init();
		CURLcode res = CURLE_OK;
		if (curl) {

			curl_easy_setopt(curl, CURLOPT_URL, url.data());

			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
		}
		return res == CURLE_OK;
	}

	/* This remains untested (1/1/17) : */
	bool post(const std::string& url, const nlohmann::json& params, std::string& response) {
		std::cout << "MyHTTP::post( " << url << " , " << params << " )\n";

		CURL *curl = curl_easy_init();
		CURLcode res = CURLE_OK;

		if (curl) {

			curl_easy_setopt(curl, CURLOPT_URL, url.data());

			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

			std::string p = "";
			_parse_params(params, p);
			if (p.empty()) {
				curl_easy_cleanup(curl);
				return false;
			}
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, p.data());

			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
		}
		return res == CURLE_OK;
	}

	std::string uri_encode(const std::string& unformatted) {

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

};
