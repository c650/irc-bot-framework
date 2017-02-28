#include <iostream>
#include <curl/curl.h>
#include <cstring>
#include <stdexcept>

#include "./include/json.hpp"
#include "./include/http.hpp"

namespace MyHTTP {

/*
	HELPER
	Prototypes.
*/
	/* to store result of GET/POST in stream */
	static size_t write_to_string(void *ptr, size_t size, size_t nmemb, std::string& stream);

	/* ideally, converts JSON params into a string-form and stores it in p. */
	static void _parse_params(const nlohmann::json& params, std::string& p);

/* ------------*/

	/* see .hpp for docs */
	std::string get(const std::string& url) {

		#ifdef DEBUG
			std::cout << "MyHTTP::get( " << url << " )\n";
		#endif

		std::string response;

		CURLHandle curl_handle;
		CURLcode res = curl_handle.do_setup(url, write_to_string, response).perform();
		if (res != CURLE_OK)
			throw std::runtime_error("HTTP GET of " + url + " failed with CURLcode: " + std::to_string(res));

		return response;
	}

	/* untested (2/24/17) */
	std::string post(const std::string& url, const std::string& params) {
		#ifdef DEBUG
			std::cout << "MyHTTP::post( " << url << " , " << params << " )\n";
		#endif

		std::string response;

		CURLHandle curl_handle;
		curl_handle.do_setup(url, write_to_string, response);
		curl_easy_setopt(curl_handle.curl_ptr, CURLOPT_POSTFIELDS, params.data());

		CURLcode res = curl_handle.perform();
		if (res != CURLE_OK)
			throw std::runtime_error("HTTP POST of " + url + " with params: " + params + " failed. CURLcode: " + std::to_string(res));

		return response;
	}

	/* This remains untested (1/1/17) : */
	std::string post(const std::string& url, const nlohmann::json& params) {
		std::string p = "";
		_parse_params(params, p);
		return post(url, p); /* calls other one. */
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

	/* CURLHandle Implementation */

	CURLHandle::CURLHandle() : CURLHandle(nullptr) {}
	CURLHandle::CURLHandle(CURL* _curl_ptr) : curl_ptr(_curl_ptr) {}

	CURLHandle::~CURLHandle() {
		if (this->curl_ptr != nullptr)
			curl_easy_cleanup(this->curl_ptr);
	}

	CURLcode CURLHandle::perform(void) {
		if (this->curl_ptr == nullptr)
			throw std::logic_error("Cannot perform the CURL action on a nullptr!");
		return curl_easy_perform(this->curl_ptr);
	}


};
