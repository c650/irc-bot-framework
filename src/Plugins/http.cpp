#include <iostream>
#include <curl/curl.h>
#include <cstring>
#include <stdexcept>

#include "./include/json.hpp"

namespace MyHTTP {

/*
	HELPER
	Prototypes.
*/
	/* to store result of GET/POST in stream */
	static size_t write_to_string(void *ptr, size_t size, size_t nmemb, std::string& stream);

	template <typename WriteFunc> /* gotta love templates! */
	static CURL* do_curl_setup(const std::string& url, WriteFunc func, std::string& response_string);

	/* ideally, converts JSON params into a string-form and stores it in p. */
	static void _parse_params(const nlohmann::json& params, std::string& p);

/* ------------*/

	/* see .hpp for docs */
	bool get(const std::string& url, std::string& response) {

		#ifdef DEBUG
			std::cout << "MyHTTP::get( " << url << " )\n";
		#endif

		CURL *curl = do_curl_setup(url, write_to_string, response);
		if (curl) {
			CURLcode res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
			return res == CURLE_OK;
		}
		return false;
	}

	/* untested (2/24/17) */
	bool post(const std::string& url, const std::string& params, std::string& response) {
		#ifdef DEBUG
			std::cout << "MyHTTP::post( " << url << " , " << params << " )\n";
		#endif

		CURL *curl = do_curl_setup(url, write_to_string, response);
		if (curl) {
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, params.data());

			CURLcode res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);

			return res == CURLE_OK;
		}
		return false;
	}

	/* This remains untested (1/1/17) : */
	bool post(const std::string& url, const nlohmann::json& params, std::string& response) {
		std::string p = "";
		_parse_params(params, p);
		return post(url, p, response); /* calls other one. */
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

/* -----------------*/
	/* HELPERS */

	static size_t write_to_string(void *ptr, size_t size, size_t nmemb, std::string& stream) {
		size_t realsize = size * nmemb;
		std::string temp(static_cast<const char*>(ptr), realsize);
		stream.append(temp);
		return realsize;
	}

	template <typename WriteFunc>
	static CURL* do_curl_setup(const std::string& url, WriteFunc func, std::string& response_string) {
		CURL *curl = curl_easy_init();
		if (curl) {
			curl_easy_setopt(curl, CURLOPT_URL, url.data());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, func);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_string);
			return curl;
		}
		return nullptr;
	}

	static void _parse_params(const nlohmann::json& params, std::string& p) {
		/*
		try {
			for (auto element = params.begin(); element != params.end() ; ++element) {
				p.append((*element).key().get<std::string>() + "=" + (*element).value().get<std::string>() + "&");
			}
			p.pop_back(); // remove last '&'
		} catch (std::exception& e) {
			std::cerr << "Failed to parse params: " << e.what() << '\n';
			p = "";
		}
		*/
		p="";
	}

};
